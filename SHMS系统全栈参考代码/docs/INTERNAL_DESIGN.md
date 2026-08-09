# 服务端内部设计文档

## 1. 设计目标

服务端负责把 Qt 客户端、摄像头设备和 MySQL 数据库连接起来，提供统一的 TLV 协议入口。核心目标包括：

- 管理用户登录、注册和连接会话。
- 维护摄像头配置缓存，向客户端下发设备列表。
- 转发实时 RTSP 流，并把媒体数据封装成 TLV 返回客户端。
- 管理云台控制权，避免多个客户端同时控制同一摄像头。
- 自动录制摄像头视频，索引录像分片，并支持查询和回放。
- 提供服务端日志读取能力，方便 GUI 查看运行状态。

## 2. 启动流程

服务端入口是 `src/main.cpp`，默认读取 `conf/server.conf`。

```text
读取配置
  -> 初始化 Logger
  -> 配置 MediaStreamManager
  -> 配置 RecordManager
  -> 初始化 MySQLConnectionPool
  -> CameraRegistry 从数据库加载摄像头
  -> RecordManager 启动自动录像
  -> MonitorServer 启动 TCP 监听
```

关键配置项：

| 配置 | 默认值 | 作用 |
| --- | --- | --- |
| `ip` | `0.0.0.0` | TCP 监听地址 |
| `port` | `8123` | TLV 服务端口 |
| `thread_num` | `4` | 业务线程池线程数，也是 MySQL 连接池大小 |
| `task_num` | `10000` | 业务线程池任务队列上限 |
| `stream_packet_queue_size` | `256` | 实时流转发内部包队列长度 |
| `stream_send_queue_bytes` | `16777216` | 单个实时流发送缓冲上限 |
| `stream_fragment_size` | `61440` | TLV 媒体分片大小 |
| `record_path` | `./data/records` | 录像分片目录 |
| `record_segment_seconds` | `10` | 自动录像分片时长 |
| `record_reconnect_seconds` | `5` | 录像源断开后的重连间隔 |
| `record_chunk_size` | `262144` | 录像回放分块大小 |
| `record_download_timeout_ms` | `15000` | 外部录像源下载超时 |

## 3. 分层架构

```text
Qt Client
  |
  | TCP + custom TLV
  v
MonitorServer
  |-- Network / TcpServer / EventLoop
  |-- per-connection TLV::Codec
  |-- ThreadPool
  v
BusinessService
  |-- UserSessionManager
  |-- CameraRegistry
  |-- PtzControlQueue + CameraControlGateway
  |-- MediaStreamManager
  |-- RecordManager
  |-- MySQLConnectionPool
```

模块职责：

| 模块 | 文件 | 职责 |
| --- | --- | --- |
| 网络层 | `include/Network.h`, `src/Network.cpp` | TCP accept、连接读写、事件循环 |
| 服务入口 | `include/MonitorServer.h`, `src/MonitorServer.cpp` | 连接生命周期、TLV 解码、线程池分发 |
| 业务分发 | `include/BusinessService.h`, `src/BusinessService.cpp` | 按 TLV 类型路由登录、云台、设备、日志等请求 |
| 会话管理 | `UserSessionManager` | 维护 `connectionId -> username` 登录态 |
| 摄像头注册表 | `include/CameraRegistry.h`, `src/CameraRegistry.cpp` | 从 MySQL 加载摄像头配置并提供内存查询 |
| 云台控制 | `include/CameraControl.h`, `src/CameraControl.cpp` | 控制权排队、租约、HTTP 云台请求转发 |
| 实时流 | `include/MediaStreamForwarder.h`, `src/MediaStreamForwarder.cpp` | 打开 RTSP，转发 metadata 和 packet |
| 录像 | `include/RecordManager.h`, `src/RecordManager.cpp` | 自动录像、录像索引、录像查询和回放 |
| 数据库 | `include/Database.h`, `src/Database.cpp` | MySQL 连接池和 SQL 查询封装 |
| 协议 | `include/TLV/*` | TLV 编解码、类型定义、消息结构 |

## 4. 请求处理模型

`MonitorServer` 为每个连接保存一个独立的 `TLV::Codec`。客户端数据到达后：

1. `onMessage` 按连接 fd 找到对应 codec。
2. codec 追加字节流并解析出一个或多个完整 TLV 消息。
3. 每个消息封装成 `MonitorTask` 提交到线程池。
4. `MonitorTask::process` 先处理实时流和录像回放类长任务。
5. 其他短请求调用 `BusinessService::dispatch`。
6. 业务返回单个 TLV 响应，由连接对象发回客户端。

`MonitorTask::process` 的特殊路由：

| TLV 类型 | 处理模块 | 原因 |
| --- | --- | --- |
| `START_STREAM` / `STOP_STREAM` | `MediaStreamManager` | 需要拿到 `TcpConnectionPtr`，并启动或停止连接级实时流上下文 |
| `START_RECORD_PLAYBACK` / `STOP_RECORD_PLAYBACK` | `RecordManager` | 回放需要持有连接对象并在独立线程中连续发送 `RECORD_CHUNK` |
| 其他业务消息 | `BusinessService` | 请求短、返回单个同步响应 |

设计取舍：

- TCP 粘包、拆包只在 `TLV::Codec` 处理，业务层只接触完整消息。
- 普通请求使用线程池处理，避免阻塞网络事件循环。
- 实时流和录像回放是长任务，由 `MediaStreamManager` / `RecordManager` 独立创建工作线程。
- 线程池满时直接返回 `ERROR: server busy`，保护服务端不被请求堆积拖垮。

连接关闭时会做统一清理：

```text
删除连接对应 TLV codec
  -> 停止该连接的实时流
  -> 停止该连接的录像回放
  -> 按用户名释放云台控制权和排队项
  -> 删除登录会话
```

## 5. TLV 协议设计

TLV 头部由 `MAGIC + type + length` 组成，最大 value 长度为 `1MB`。复杂消息使用嵌套 TLV，例如登录消息的 value 内部包含 `USERNAME` 和 `PASSWORD`。

主要消息：

| TLV 类型 | 方向 | 用途 |
| --- | --- | --- |
| `LOGIN` / `REGISTER` | client -> server | 登录和注册 |
| `TOKEN` | server -> client | 登录或注册成功 |
| `GET_CAMERAS` / `CAMERA_LIST` | 双向 | 获取摄像头列表 |
| `PTZ_ACQUIRE` / `PTZ_RELEASE` | client -> server | 申请和释放云台控制权 |
| `PTZ_CONTROL` | client -> server | 发送云台动作 |
| `PTZ_RESULT` / `PTZ_BUSY` | server -> client | 云台成功、排队或占用反馈 |
| `START_STREAM` / `STOP_STREAM` | client -> server | 开始或停止实时流 |
| `STREAM_METADATA` / `STREAM_PACKET` / `STREAM_ERROR` | server -> client | 实时流元数据、媒体包、错误 |
| `QUERY_RECORDS` / `RECORD_LIST` | 双向 | 查询录像分片 |
| `START_RECORD_PLAYBACK` / `STOP_RECORD_PLAYBACK` | client -> server | 开始或停止录像回放 |
| `RECORD_METADATA` / `RECORD_CHUNK` / `RECORD_ERROR` | server -> client | 回放元数据、分块、错误 |
| `GET_RECORD_CONFIG` / `SET_RECORD_CONFIG` / `RECORD_CONFIG` | 双向 | 查看或修改录像配置 |
| `GET_LOGS` / `LOG_CONTENT` | 双向 | 读取服务端日志 |

## 6. 用户与会话设计

用户数据存储在 MySQL `users` 表：

- `name` 是唯一用户名。
- `setting` 是随机盐。
- `encrypt` 保存 `MD5(CONCAT(plain_password, setting))`。
- `status=1` 表示账号可用。
- 登录成功会更新 `last_login_at`。

服务端运行时使用 `UserSessionManager` 维护登录态。除登录、注册、心跳等少量入口外，摄像头列表、云台、录像、日志等业务都要求连接已经登录。

注册校验规则：

- 用户名长度 `3-20`。
- 用户名只允许字母、数字、下划线和点号。
- 注册密码长度 `6-64`。
- 用户名和密码都不能包含控制字符。

## 7. 摄像头注册表

`CameraRegistry` 在服务端启动时从 `cameras` 表加载配置，并缓存在内存中：

| 字段 | 作用 |
| --- | --- |
| `name`, `channel` | 摄像头唯一键 |
| `rtsp_url` | 实时流和自动录像输入源 |
| `ptz_request_template` | 云台 HTTP 请求模板，空值表示只可观看 |
| `ptz_method` | 云台请求方法，默认 `GET` |
| `ptz_body_template` | 云台请求 body 模板 |
| `record_enabled` | 是否启动自动录像 |
| `record_provider` | 录像来源类型，默认 `xsw` |
| `record_base_url`, `record_secret` | 外部录像接口配置 |

注册表提供两个查询入口：

- `find`：查任意摄像头通道，用于播放、录像等。
- `findControllable`：只返回配置了云台模板的通道，用于云台控制。

当前设计是启动时加载一次。修改数据库后，如果没有显式刷新逻辑，需要重启服务端或通过后续接口扩展触发刷新。

## 8. 云台控制权设计

云台设计拆成两部分：

- `PtzControlQueue`：负责每个摄像头通道的控制权、等待队列和租约。
- `CameraControlGateway`：负责把抽象云台动作转换为摄像头 HTTP 请求。

控制权状态以 `cameraName:channel` 为 key，每个 key 有一个 `CameraState`：

| 状态 | 说明 |
| --- | --- |
| `ownerId` | 当前持有控制权的用户名 |
| `leaseUntil` | 控制权租约过期时间 |
| `waiters` | 等待控制权的用户名队列 |

申请流程：

```text
PTZ_ACQUIRE
  -> 校验登录态
  -> 解析 cameraName/channel
  -> 检查摄像头存在且支持云台
  -> 如果无 owner 且队列为空，授予控制权并返回 PTZ_RESULT: granted
  -> 否则加入等待队列并返回 PTZ_BUSY: camera is busy, owner=..., ahead=N
```

执行流程：

```text
PTZ_CONTROL
  -> 校验登录态
  -> 解析 cameraName/channel/action/speed/duration/preset
  -> 检查该用户是否为当前 owner
  -> 不是 owner 时返回 PTZ_BUSY
  -> 是 owner 时通过 CameraControlGateway 发送 HTTP 请求
  -> 成功后续租控制权，返回 PTZ_RESULT: ok
```

释放流程：

```text
PTZ_RELEASE 或连接关闭 / 登出
  -> 如果用户是 owner，清空 owner 和 leaseUntil
  -> 如果用户在 waiters 中，从队列移除
  -> 唤醒等待状态
```

HTTP 请求模板支持按命令替换变量。服务端把 `up/down/left/right/up_left/up_right/down_left/down_right/zoom_in/zoom_out/stop/home/preset_call/preset_set` 等动作标准化后转发给摄像头接口。带 `durationMs` 的动作会按配置自动补发停止动作，避免摄像头持续运动。

## 9. 实时流转发设计

实时流入口是 `MediaStreamManager::startStream`：

```text
START_STREAM
  -> 校验登录态
  -> 解析 cameraName/channel
  -> 从 CameraRegistry 取 rtsp_url
  -> 创建 StreamContext
  -> 分配 streamId
  -> 启动独立线程 runStream
```

`runStream` 使用 FFmpeg 打开 RTSP：

1. `avformat_open_input` 打开摄像头流。
2. `avformat_find_stream_info` 获取音视频轨道。
3. 发送 `STREAM_METADATA`。
4. 循环读取 `AVPacket`。
5. 将音视频包序列化为媒体协议包。
6. 按 `stream_fragment_size` 拆成 TLV 分片发送 `STREAM_PACKET`。

为了避免读流线程被网络发送阻塞，内部有有界媒体包队列。停止流或连接关闭时设置 `stopRequested`，并从 `_streamsByConnection` 移除上下文。

约束：

- 当前每个连接只保存一个实时流上下文；同一连接再次开始流会替换旧上下文。
- 摄像头 URL 必须是 `rtsp://`。
- 实时流错误通过 `STREAM_ERROR` 返回客户端。

## 10. 录像设计

录像入口是 `RecordManager`，它同时处理自动录像、录像查询和录像回放。

自动录像：

```text
startAutoRecording
  -> 遍历 CameraRegistry
  -> 对 record_enabled=1 的摄像头创建 RecordingContext
  -> 独立线程打开 RTSP 或外部录像源
  -> 按 record_segment_seconds 切分 TS 文件
  -> 写入 record_segments 索引
```

录像文件落盘策略：

- 分片先以写入中状态生成临时文件。
- 分片完成后计算文件大小和 MD5。
- 成功后写入或更新 `record_segments`。
- 启动时会扫描 `record_path`，恢复已完成或异常中断的分片索引。

录像查询：

```text
QUERY_RECORDS
  -> 校验登录态
  -> 解析 cameraName/channel/start/end
  -> 查 record_segments
  -> 返回 RECORD_LIST
```

录像回放：

```text
START_RECORD_PLAYBACK
  -> 校验登录态
  -> 查询时间范围内分片
  -> 分配 playbackId
  -> 发送 RECORD_METADATA
  -> 按 record_chunk_size 读取文件
  -> 连续发送 RECORD_CHUNK
```

连接关闭时 `stopByConnection` 会停止该连接对应的回放任务。

## 11. 数据库设计

核心表：

| 表 | 作用 |
| --- | --- |
| `users` | 用户账号、密码盐、密码摘要、状态和最后登录时间 |
| `cameras` | 摄像头通道、RTSP 地址、云台模板、录像配置 |
| `record_segments` | 录像分片索引、时间范围、文件路径、大小、MD5 和状态 |

重要索引：

| 索引 | 表 | 作用 |
| --- | --- | --- |
| `uk_users_name` | `users` | 保证用户名唯一 |
| `uk_cameras_name_channel` | `cameras` | 保证摄像头通道唯一 |
| `idx_record_lookup` | `record_segments` | 按摄像头、通道和时间查询录像 |
| `idx_record_status` | `record_segments` | 扫描异常或待处理分片 |
| `idx_record_camera_time` | `record_segments` | 按摄像头名称和时间查询 |

`schema.sql` 带有 `ensure_camera_column` 过程，用于给旧库补齐云台和录像相关字段。

## 12. 线程与资源管理

主要线程来源：

| 来源 | 数量 | 说明 |
| --- | --- | --- |
| 网络事件循环 | 由 `TcpServer` 管理 | 负责连接事件和基础读写 |
| `ThreadPool` | `thread_num` | 处理短业务请求 |
| 实时流线程 | 每个活动连接最多一个 | 读取 RTSP 并转发实时媒体 |
| 实时流发送线程 | 每个实时流内部一个 | 从有界队列取包并发送 |
| 自动录像线程 | 每个启用录像的摄像头一个 | 持续录制并切片 |
| 录像回放线程 | 每个活动回放一个 | 分块读取录像文件并发送 |

资源清理原则：

- 连接关闭时按连接 fd 清理 codec、实时流和回放。
- 登录用户退出或断线时按 username 释放云台 owner 和 waiters。
- 长任务通过 `atomic<bool> stopRequested` 协作退出。
- 共享状态使用 mutex 保护，避免网络线程、业务线程和长任务线程并发修改。

## 13. 错误处理与日志

服务端统一使用 `Logger` 写入 `log_file`。GUI 可通过 `GET_LOGS` 拉取最近日志内容，服务端最多返回 `512KB`。

常见错误返回：

| 场景 | 返回 |
| --- | --- |
| 未登录访问受保护接口 | `ERROR: login required` |
| TLV 格式错误 | `ERROR: invalid ... format` |
| 线程池满 | `ERROR: server busy` |
| 摄像头不存在 | `ERROR: camera not found` |
| 摄像头不支持云台 | `ERROR: camera is view-only...` |
| 云台被占用 | `PTZ_BUSY: camera is busy, owner=..., ahead=N` |
| RTSP 打开失败 | `STREAM_ERROR` 或录像日志错误 |
| 录像查询无数据 | 返回空 `RECORD_LIST` |

## 14. 当前边界与后续扩展

当前实现中需要注意的边界：

- MySQL 连接参数在 `src/main.cpp` 固定为 `127.0.0.1 / monitor / 1234 / monitor`，不是从配置文件读取。
- 摄像头注册表启动时加载，运行期修改摄像头配置后需要刷新机制或重启。
- 云台 owner 使用 username 标识，同一账号多端登录时会共享 owner 身份。
- 实时流当前按连接维度管理一个流上下文，如果要单连接多路流，需要把 `_streamsByConnection` 改成按 `connectionId + streamId` 管理。
- 密码摘要使用 MD5 加盐，满足课程或演示场景；生产环境应替换为 bcrypt、Argon2 等慢哈希。
