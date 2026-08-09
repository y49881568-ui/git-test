# Server 端任务注册与分发说明

本文说明 server 端“任务”是如何进入系统、如何被线程池执行、以及如何路由到具体业务处理函数的。

## 1. 结论

当前 server 端没有独立的任务注册中心，也没有类似 `registerTask(type, handler)` 的静态注册表。

它采用的是：

```text
注册 TCP 回调
  -> 收到客户端 TLV 数据
  -> 解码成一个或多个 TLV::Message
  -> 每个 Message 封装成 MonitorTask
  -> 提交 ThreadPool
  -> MonitorTask::process 按 TLV 类型分发
```

因此这里的“注册任务”可以分成两层理解：

- 网络事件注册：`MonitorServer` 把连接、消息、关闭回调注册到 `TcpServer`。
- 业务任务提交：每收到一个完整 TLV 消息，就动态创建一个 `MonitorTask` 并提交给线程池。

## 2. 启动时创建线程池

入口文件：`server/src/main.cpp`

服务端启动时读取配置：

```cpp
int threadNum = config.getInt("thread_num", 4);
int taskNum = config.getInt("task_num", 10000);
```

然后构造 `MonitorServer`：

```cpp
MonitorServer server(ip, static_cast<unsigned short>(port),
                     static_cast<size_t>(threadNum),
                     static_cast<size_t>(taskNum), videoPath);
```

含义：

- `thread_num`：业务线程池 worker 数量。
- `task_num`：线程池任务队列最大长度。

`MonitorServer` 构造函数里初始化线程池：

```cpp
MonitorServer::MonitorServer(...)
    : _threadPool(threadNum, taskQueueSize), _server(port, ip),
      _videoPath(videoPath) {
  ...
}
```

## 3. 注册网络事件回调

文件：`server/src/MonitorServer.cpp`

`MonitorServer` 构造时调用：

```cpp
_server.setAllCallbacks(
    std::bind(&MonitorServer::onConnection, this, _1),
    std::bind(&MonitorServer::onMessage, this, _1, _2),
    std::bind(&MonitorServer::onClose, this, _1));
```

这里注册的是 TCP 层事件：

| 回调 | 触发时机 | 作用 |
| --- | --- | --- |
| `onConnection` | 新客户端连接 | 记录连接日志 |
| `onMessage` | 连接收到字节流 | 解码 TLV 并提交业务任务 |
| `onClose` | 连接关闭 | 清理 codec、流、回放、会话和云台控制权 |

真正的业务任务是在 `onMessage` 里动态生成的。

## 4. 收到消息后创建 MonitorTask

文件：`server/src/MonitorServer.cpp`

`onMessage` 收到的是 TCP 字节流，先按连接 fd 找到对应的 `TLV::Codec`：

```cpp
TLV::Codec &codec = _codecs[conn->fd()];
codec.append(data);
messages = codec.decodeAll();
```

`decodeAll()` 可能一次解出多个完整 TLV 消息，因为 TCP 可能粘包。

随后每个消息都会封装成一个任务：

```cpp
for (const auto &message : messages) {
  MonitorTask task(conn, message);
  bool accepted =
      _threadPool.submit(std::bind(&MonitorTask::process, task));

  if (!accepted) {
    sendError(conn, "server busy");
  }
}
```

这里就是业务任务进入 server 的核心位置。

如果线程池队列已满，`submit()` 返回 `false`，server 会给客户端返回：

```text
ERROR: server busy
```

## 5. ThreadPool 如何接收和执行任务

文件：`server/src/ThreadPool.cpp`

提交任务：

```cpp
bool ThreadPool::submit(std::function<void()> task) {
  {
    std::lock_guard<std::mutex> lock(mtx_);

    if (stop_) {
      return false;
    }

    if (maxQueueSize_ > 0 && tasks_.size() >= maxQueueSize_) {
      return false;
    }

    tasks_.push(std::move(task));
  }

  cv_.notify_one();
  return true;
}
```

执行任务：

```cpp
task = std::move(tasks_.front());
tasks_.pop();
task();
```

线程池只关心 `std::function<void()>`，不理解 TLV 类型，也不理解业务含义。

## 6. MonitorTask 如何分发业务

文件：`server/src/BusinessService.cpp`

`MonitorTask::process()` 是任务执行入口：

```cpp
void MonitorTask::process() {
  if (_message.type() == TLV::Type::START_STREAM) {
    sendMessage(MediaStreamManager::instance().startStream(_conn, _message));
  } else if (_message.type() == TLV::Type::STOP_STREAM) {
    sendMessage(
        MediaStreamManager::instance().stopStream(_conn->fd(), _message));
  } else if (_message.type() == TLV::Type::START_RECORD_PLAYBACK) {
    sendMessage(RecordManager::instance().startPlayback(_conn, _message));
  } else if (_message.type() == TLV::Type::STOP_RECORD_PLAYBACK) {
    sendMessage(RecordManager::instance().stopPlayback(_conn->fd()));
  } else {
    sendMessage(_service.dispatch(_message, _conn->fd()));
  }
}
```

这里有两类处理路径。

### 6.1 长任务特殊处理

以下 TLV 类型不直接交给 `BusinessService::dispatch()`：

| TLV 类型 | 处理模块 | 原因 |
| --- | --- | --- |
| `START_STREAM` | `MediaStreamManager` | 实时流需要持有连接并持续发送媒体包 |
| `STOP_STREAM` | `MediaStreamManager` | 按连接停止实时流上下文 |
| `START_RECORD_PLAYBACK` | `RecordManager` | 录像回放需要持续发送 `RECORD_CHUNK` |
| `STOP_RECORD_PLAYBACK` | `RecordManager` | 按连接停止回放任务 |

这些请求虽然最初也通过 `MonitorTask` 进入线程池，但真正的持续工作会由对应 manager 维护独立上下文或线程。

### 6.2 普通短请求分发

普通请求进入：

```cpp
_service.dispatch(_message, _conn->fd())
```

`BusinessService::dispatch()` 通过 `switch` 按 TLV 类型路由：

```cpp
switch (message.type()) {
case TLV::Type::HEARTBEAT:
  return TLV::Message(TLV::Type::HEARTBEAT, "pong");
case TLV::Type::LOGIN:
  return handleLogin(message, connectionId);
case TLV::Type::REGISTER:
  return handleRegister(message, connectionId);
case TLV::Type::GET_CAMERAS:
  return handleGetCameras(connectionId);
...
default:
  return TLV::Message(TLV::Type::ERROR, "unsupported message type");
}
```

这就是业务处理函数的“注册方式”：不是注册到表里，而是写在 `dispatch()` 的 `switch` 分支里。

## 7. 新增一个业务任务应该改哪里

如果要新增一个普通短请求，例如 `GET_SERVER_STATUS`，通常需要改这些地方：

1. 在 `server/include/TLV/TLVTypes.h` 新增 TLV 类型。
2. 在客户端对应 TLV 类型定义里同步新增类型。
3. 在 `BusinessService` 里新增处理函数声明和实现。
4. 在 `BusinessService::dispatch()` 的 `switch` 中新增 `case`。
5. 如果需要解析嵌套 TLV，在 `BusinessService.cpp` 中增加解析逻辑。
6. 如果需要测试协议编解码，补充 `server/tests` 下的测试。

普通短请求不需要改 `ThreadPool`，也不需要改 `MonitorServer::onMessage()`。

## 8. 新增长任务应该注意什么

如果新增的是持续运行的长任务，例如持续下载、持续转发、持续监控，不建议直接在 `BusinessService::dispatch()` 里阻塞执行。

更合适的模式是：

```text
MonitorTask::process
  -> 识别 START_xxx / STOP_xxx
  -> 调用 XxxManager::start(...)
  -> XxxManager 创建并保存任务上下文
  -> 长任务在线程或异步上下文中运行
  -> 连接关闭时 XxxManager::stopByConnection(fd)
```

同时还要在 `MonitorServer::onClose()` 中补充连接关闭清理，避免客户端断开后长任务继续占用资源。

现有例子：

- 实时流：`MediaStreamManager::startStream()` / `stopByConnection()`
- 录像回放：`RecordManager::startPlayback()` / `stopByConnection()`

## 9. 完整调用链

```text
main.cpp
  -> MonitorServer server(...)
  -> MonitorServer::start()
  -> TcpServer::start()

客户端发送 TLV
  -> TcpServer / EventLoop 读到数据
  -> MonitorServer::onMessage(conn, data)
  -> TLV::Codec::append(data)
  -> TLV::Codec::decodeAll()
  -> MonitorTask task(conn, message)
  -> ThreadPool::submit(bind(&MonitorTask::process, task))
  -> ThreadPool::worker_loop()
  -> MonitorTask::process()
  -> MediaStreamManager / RecordManager / BusinessService::dispatch()
  -> TcpConnection::sendInLoop(response)
```

## 10. 设计特点

- 网络线程只负责读写和解码，不直接执行业务。
- 每个完整 TLV 消息对应一个 `MonitorTask`。
- 线程池队列有上限，避免请求无限堆积。
- 普通短请求集中在 `BusinessService::dispatch()` 分发。
- 实时流和录像回放属于长任务，由专门 manager 管理生命周期。
- 连接关闭时统一清理该连接相关资源。
