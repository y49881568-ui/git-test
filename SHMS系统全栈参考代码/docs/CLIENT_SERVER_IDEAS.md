# Qt 与 Server 端实现亮点与小巧思

本文记录当前监控系统里一些比较值得说明的设计点。它不重复完整架构文档，而是聚焦代码里一些小但实用的实现选择。

## 1. 协议层：简单 TLV 扛住多类业务

项目没有为登录、云台、实时流、录像、日志分别设计不同协议，而是统一走 TLV：

```text
MAGIC + type + length + value
```

这个选择有几个好处：

- TCP 粘包、拆包统一由 codec 处理。
- 普通字段可以用嵌套 TLV 表示，例如 `LOGIN` 的 value 里放 `USERNAME` 和 `PASSWORD`。
- 新增业务时通常只需要新增 TLV 类型和对应解析逻辑。
- Qt 端和 server 端都可以用同一套“按 type 分发”的模型。

server 端入口在：

- `server/include/TLV/TLVTypes.h`
- `server/src/TLVCodec.cpp`

Qt 端入口在：

- `monitor-client/api/TlvClient.cpp`

## 2. Server：每个连接一个 TLV Codec

`MonitorServer` 为每个连接 fd 保存独立的 `TLV::Codec`：

```cpp
TLV::Codec &codec = _codecs[conn->fd()];
codec.append(data);
messages = codec.decodeAll();
```

这是一个很实用的小设计：

- 每个 TCP 连接都有自己的半包缓存。
- 不同客户端的粘包、半包状态互不影响。
- 业务层只看到完整的 `TLV::Message`，不用关心网络字节流问题。

相关文件：

- `server/src/MonitorServer.cpp`

## 3. Server：网络线程和业务线程解耦

收到完整 TLV 消息后，server 不在网络回调里直接执行业务，而是封装成 `MonitorTask` 丢进线程池：

```cpp
MonitorTask task(conn, message);
_threadPool.submit(std::bind(&MonitorTask::process, task));
```

这个设计让网络事件循环保持轻量：

- 网络层负责 accept、read、write、codec。
- 业务层在线程池里执行。
- 队列满时返回 `server busy`，避免请求无限堆积。

相关文件：

- `server/src/MonitorServer.cpp`
- `server/src/ThreadPool.cpp`
- `server/src/BusinessService.cpp`

## 4. Server：长任务从普通 dispatch 里分离

`MonitorTask::process()` 对实时流和录像回放做了特殊路由：

```cpp
START_STREAM             -> MediaStreamManager
STOP_STREAM              -> MediaStreamManager
START_RECORD_PLAYBACK    -> RecordManager
STOP_RECORD_PLAYBACK     -> RecordManager
其他请求                 -> BusinessService::dispatch()
```

这个分离很关键：

- 登录、查询摄像头、查询日志这类请求是短请求，适合同步返回一个 TLV。
- 实时流、录像回放是持续发送数据的长任务，不能占着业务线程池一直跑。
- 长任务由 manager 保存上下文，通过 `stopByConnection()` 清理生命周期。

相关文件：

- `server/src/BusinessService.cpp`
- `server/src/MediaStreamForwarder.cpp`
- `server/src/RecordManager.cpp`

## 5. Server：连接关闭时统一回收资源

`MonitorServer::onClose()` 做的不只是删连接，而是统一回收连接关联资源：

```text
删除 TLV codec
  -> 停止实时流
  -> 停止录像回放
  -> 释放云台控制权
  -> 删除登录会话
```

这避免了几类常见问题：

- 客户端断开后推流线程继续跑。
- 录像回放还在给失效连接发数据。
- 用户异常退出后云台控制权一直占用。
- 会话状态残留。

相关文件：

- `server/src/MonitorServer.cpp`

## 6. Server：实时流做分片和发送背压

实时流不是直接把大块媒体数据塞进 TLV，而是按配置的 `stream_fragment_size` 切分：

```text
payload
  -> fragmentIndex / fragmentCount
  -> 多个 STREAM_PACKET 或 STREAM_METADATA
```

server 端还会检查连接待发送字节数：

```cpp
conn->pendingOutputBytes()
```

这个设计的价值：

- 避免单个 TLV value 过大。
- 大媒体包可以拆成多个稳定大小的片段。
- 客户端可以按 `streamId + mediaType + payloadKind + sequence` 重组。
- 发送队列过大时可以保护 server，不让慢客户端拖垮内存。

相关文件：

- `server/include/MediaStreamProtocol.h`
- `server/src/MediaStreamProtocol.cpp`
- `server/src/MediaStreamForwarder.cpp`
- `monitor-client/video/StreamProtocolCodec.cpp`

## 7. Qt：StreamReassembler 按复合 key 重组分片

Qt 端的 `StreamReassembler` 没有只按 sequence 重组，而是使用复合 key：

```text
streamId:mediaType:payloadKind:sequence
```

这样可以避免不同流、不同媒体类型、metadata 和 packet 之间互相串片。

重组完成后才继续解析：

```text
STREAM_METADATA -> parseMetadata()
STREAM_PACKET   -> parsePacket()
```

相关文件：

- `monitor-client/video/StreamProtocolCodec.cpp`
- `monitor-client/api/TlvClient.cpp`

## 8. Qt：MonitorApiClient 把连接和认证串成一个动作

Qt 端没有让 UI 自己处理“先连接 socket，再等 connected，再发 login/register”的细节，而是在 `MonitorApiClient` 里保存 pending action：

```cpp
startAuth(PendingAction::Login, username, password);
...
flushPendingAuth();
```

效果是 UI 可以直接调用：

```cpp
connectAndLogin(host, port, username, password);
connectAndRegister(host, port, username, password);
```

这降低了 UI 复杂度：

- 如果 socket 已连接，立即发认证请求。
- 如果还没连接，等 `connected` 信号后自动补发。
- 断开连接时清空 pending 状态，避免旧认证请求误发。

相关文件：

- `monitor-client/api/MonitorApiClient.cpp`

## 9. Qt：TlvClient 把协议响应转换成业务信号

`TlvClient::handleMessage()` 把底层 TLV 类型转换成 Qt 信号：

```text
TOKEN         -> loginSucceeded / registerSucceeded / ptzSucceeded
CAMERA_LIST   -> camerasReceived
STREAM_PACKET -> streamPacketReceived
RECORD_CHUNK  -> recordChunkReceived
ERROR         -> loginFailed / registerFailed / ptzFailed / errorReceived
```

这里的一个小巧思是 `_pendingAuth`：

- 同样的 `TOKEN` 可以表示登录成功、注册成功、云台申请成功。
- 同样的 `ERROR` 也可以映射成不同业务失败信号。
- UI 层不用自己根据最近发过什么请求去判断响应含义。

相关文件：

- `monitor-client/api/TlvClient.cpp`

## 10. Qt：退出窗口时先发 LOGOUT 再断开

Qt 端退出不是直接关闭 socket，而是：

```cpp
sendMessage(LOGOUT);
_socket->flush();
_socket->disconnectFromHost();
```

如果 socket 状态异常，再走 `abort()`。

这个处理比较细：

- 正常退出时 server 能收到 `LOGOUT`。
- server 可以主动清理会话。
- 异常状态下不会卡在优雅关闭流程里。

相关文件：

- `monitor-client/api/TlvClient.cpp`

## 11. 云台：控制权、排队和租约

云台控制不是简单收到命令就转发，而是有 `PtzControlQueue`：

```text
cameraKey -> CameraState
  -> ownerId
  -> leaseUntil
  -> waiters
```

这个设计解决了多个客户端同时控制同一摄像头的问题：

- 同一摄像头同一时间只有一个 owner。
- 后来的用户进入等待队列。
- owner 有租约时间，避免长期占用。
- 用户断开时通过 `releaseByOwner()` 释放控制权。

相关文件：

- `server/include/CameraControl.h`
- `server/src/CameraControl.cpp`
- `server/src/BusinessService.cpp`

## 12. 云台：duration 自动 stop

`PtzCommand` 支持 `durationMs`。对于需要持续动作的命令，server 可以在发送动作后再构造 stop 命令。

这让客户端可以发一次：

```text
向左移动 300ms
```

而不是必须手动发：

```text
left -> sleep -> stop
```

好处：

- 控制动作更接近“一次指令”。
- 客户端 UI 不需要自己维护所有 stop 定时逻辑。
- server 更容易保证不同客户端争用时的动作边界。

相关文件：

- `server/include/CameraControl.h`
- `server/src/CameraControl.cpp`

## 13. Qt：方向键时长做了体验补偿

主窗口里对云台方向命令做了时长缩放：

- 垂直方向使用单独比例。
- 斜向移动按近似 `1 / sqrt(2)` 做修正。

这是一个偏 UI 手感的小点：同样的按键时间下，斜向移动不会明显比单轴移动“走得更远”。

相关文件：

- `monitor-client/mainwindow.cpp`

## 14. 录像：自动录制和配置修改联动

server 启动时：

```text
CameraRegistry refresh
  -> RecordManager::startAutoRecording()
```

修改录像配置后：

```text
保存数据库
  -> CameraRegistry refresh
  -> 停止旧录制上下文
  -> startAutoRecording()
```

这让“配置变更”可以立即体现在运行任务上，不需要重启 server。

相关文件：

- `server/src/main.cpp`
- `server/src/RecordManager.cpp`
- `server/src/CameraRegistry.cpp`

## 15. 录像：先落临时文件，再改名和入库

录像分片写入时会经历类似流程：

```text
写临时文件
  -> 校验分片
  -> rename 成最终文件
  -> 写 record_segments 索引
```

这个模式可以降低半成品文件被查询到的概率：

- 写失败的文件不会以正式分片出现。
- 入库的是已经完成的分片。
- 服务重启后也可以扫描和恢复已完成但未入库的分片。

相关文件：

- `server/src/RecordManager.cpp`

## 16. 录像回放：metadata 与 chunk 分离

录像回放不是直接发文件，而是先发 metadata，再分块发送：

```text
RECORD_METADATA
  -> RECORD_CHUNK
  -> RECORD_CHUNK
  -> ...
```

这样 Qt 端可以先知道：

- playbackId
- 分片数量
- 总大小
- 时间范围

然后再逐块接收数据，适合做进度条、缓存和后续播放控制。

相关文件：

- `server/src/RecordManager.cpp`
- `server/include/RecordProtocol.h`
- `monitor-client/api/TlvClient.cpp`

## 17. Qt：录像导出时重建连续时间线

`RecordPlaybackWidget` 里导出多个录像分片时，不是简单拼文件，而是用 FFmpeg remux：

```text
逐个打开分片
  -> 复制音视频 stream 参数
  -> 将每段 pts/dts 映射到连续 ms 时间线
  -> av_interleaved_write_frame()
```

这个处理可以减少分片之间时间戳跳变导致的播放问题。

相关文件：

- `monitor-client/widgets/RecordPlaybackWidget.cpp`

## 18. 数据库：连接池大小跟业务线程数绑定

server 启动时 MySQL 连接池大小使用 `thread_num`：

```cpp
MySQLConnectionPool::instance().init(
    makeDatabaseConfig(static_cast<size_t>(threadNum)));
```

这个选择简单但合理：

- 业务请求主要在线程池中访问数据库。
- 连接池大小和并发业务线程数匹配。
- 避免线程很多但数据库连接很少，或连接很多但业务线程用不上。

相关文件：

- `server/src/main.cpp`
- `server/src/Database.cpp`

## 19. 配置项都带兜底值

server 启动读取配置后，对关键数值做了下限修正：

```cpp
if (threadNum <= 0) {
  threadNum = 4;
}
if (taskNum <= 0) {
  taskNum = 10000;
}
```

这个处理不复杂，但实际很有用：

- 配错配置不至于直接得到 0 线程或 0 队列。
- 流分片大小、录像分片时长、下载超时也都有兜底。
- demo 和部署时更抗配置错误。

相关文件：

- `server/src/main.cpp`

## 20. 一句话总结

这个项目的主要小巧思不在某个复杂算法，而在边界处理：

- TLV 统一协议入口。
- 每连接独立 codec。
- 网络线程和业务线程分离。
- 长任务由 manager 管生命周期。
- 连接关闭统一清理资源。
- 流数据分片和客户端重组。
- Qt API 把连接、认证、协议响应转换成业务信号。
- 录像和云台都考虑了运行中的状态恢复、释放和用户体验。
