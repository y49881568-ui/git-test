# 视频监控管理平台演示文档

## 1. 项目概览

本项目是一个基于 C++ 的视频监控管理平台，包含服务端和 Qt 桌面客户端两个部分。

| 目录 | 说明 |
| --- | --- |
| `server` | 监控平台服务端，负责登录注册、摄像头管理、云台控制、实时视频流转发、录像管理、日志查询 |
| `monitor-client` | Qt 桌面客户端，提供视频监控、录像回放、设备控制、系统设置等界面 |

系统通信使用自定义 TLV 协议，默认监听 `8123` 端口。服务端摄像头、用户、录像索引存储在 MySQL 数据库中。

服务端内部设计、模块职责、线程模型、云台控制权、实时流和录像链路见 `server/INTERNAL_DESIGN.md`。

## 2. 核心功能

| 功能 | 演示点 |
| --- | --- |
| 登录 / 注册 | 客户端启动后弹出登录窗口，支持账号登录和新用户注册 |
| 设备列表 | 登录成功后自动拉取摄像头列表，按摄像头和通道展示 |
| 实时监控 | 双击设备通道，或右键选择播放窗口，把视频流放入指定分屏 |
| 分屏切换 | 支持 `1 / 2 / 4 / 8 / 16` 路画面布局切换 |
| 云台控制 | 选择摄像头后可进行方向、变倍、聚焦、光圈等控制 |
| 控制权抢占 | 服务端支持云台控制权申请、释放和忙碌提示，避免多客户端同时控制 |
| 录像配置 | 可查看摄像头录像配置，并开启/关闭支持录像的通道 |
| 录像查询与回放 | 按摄像头、通道、时间范围查询录像分片，并通过客户端回放 |
| 系统设置 | 展示当前服务器 IP、端口、登录用户和连接状态，支持断开和重新登录 |

## 3. 演示环境准备

### 3.1 服务端依赖

服务端依赖：

- `g++`
- `make`
- `mysql` / `mysql_config`
- MySQL Server
- FFmpeg 开发库：`libavformat`、`libavcodec`、`libavutil`
- `python3`，用于命令行调试脚本

Ubuntu 环境可以使用项目自带脚本安装和初始化：

```bash
cd server
./scripts/setup_server.sh --install-deps --smoke-test
```

如果依赖已经安装，只初始化数据库并做冒烟测试：

```bash
cd server
./scripts/setup_server.sh --smoke-test
```

脚本会执行 `conf/schema.sql`，创建默认数据库和演示数据。

### 3.2 默认数据库和账号

| 项 | 默认值 |
| --- | --- |
| MySQL 数据库 | `monitor` |
| MySQL 用户 | `monitor` |
| MySQL 密码 | `1234` |
| 客户端登录用户 | `admin` |
| 客户端登录密码 | `1234` |

注意：客户端登录窗口代码里默认填入的是 `admin / admin123`，演示时需要手动把密码改成 `1234`。

### 3.3 默认服务端配置

服务端配置文件：`server/conf/server.conf`

```text
ip 0.0.0.0
port 8123
thread_num 4
task_num 10000
video_path ./data/
log_file ./log/server.log
record_path ./data/records
record_segment_seconds 10
```

### 3.4 默认摄像头数据

数据库脚本预置了示例摄像头：

| 摄像头 | 通道 | RTSP 地址 | 云台 |
| --- | --- | --- | --- |
| `camera_192_168_6_100` | `0` | `rtsp://admin:admin@192.168.6.100/live/chn=0` | 支持 |
| `camera_192_168_6_100` | `1` | `rtsp://admin:admin@192.168.6.100/live/chn=1` | 支持 |
| `camera_192_168_6_200` | `0` | `rtsp://admin:admin@192.168.6.200/live/chn=0` | 仅观看 |
| `camera_192_168_6_200` | `1` | `rtsp://admin:admin@192.168.6.200/live/chn=1` | 仅观看 |

实时视频、云台和录像演示依赖这些 RTSP/HTTP 摄像头在演示网络中可访问。如果没有真实摄像头，可演示登录、注册、设备列表、分屏、系统设置，以及使用脚本验证协议链路。

## 4. 启动服务端

进入服务端目录并编译：

```bash
cd server
make
```

启动服务端：

```bash
./bin/server conf/server.conf
```

服务端启动后会：

1. 读取 `conf/server.conf`
2. 初始化日志到 `log/server.log`
3. 连接 MySQL 数据库
4. 加载摄像头列表
5. 对开启录像的摄像头启动自动录像
6. 在 `8123` 端口等待客户端连接

## 5. 启动 Qt 客户端

客户端工程文件：

```text
monitor-client/Monitor_Client.pro
```

使用 Qt Creator 打开 `Monitor_Client.pro`，选择 Qt Widgets 可用的构建套件后编译运行。

Linux 下也可以使用 qmake：

```bash
cd monitor-client
qmake Monitor_Client.pro
make
./Monitor_Client
```

Windows 下如需录像回放和视频解码能力，需要配置 `FFMPEG_ROOT`。项目默认路径为：

```text
C:/libffmpeg_4.4.r101753_msvc16_x86
```

如果 FFmpeg 路径不同，需要在 qmake 配置中设置：

```bash
qmake Monitor_Client.pro FFMPEG_ROOT=C:/your/ffmpeg/path
```

## 6. 客户端演示流程

### 6.1 登录

1. 启动客户端。
2. 在登录窗口输入：
   - 用户名：`admin`
   - 密码：`1234`
3. 点击“登录”。
4. 登录成功后进入主界面，左侧显示窗口信息，中间显示视频分屏，右侧显示设备列表和云台控制。

注意：客户端当前固定连接 `192.168.6.99:8123`。如果服务端不是这个地址，需要修改 `monitor-client/mainwindow.cpp` 中的：

```cpp
constexpr const char *ServerHost = "192.168.6.99";
constexpr quint16 ServerPort = 8123;
```

### 6.2 注册新用户

1. 在登录窗口输入新用户名和密码。
2. 点击“注册”。
3. 注册成功后自动进入系统。

注册规则由服务端校验：

- 用户名不能为空
- 密码不能为空
- 用户名不能重复
- 注册密码会写入 MySQL `users` 表

### 6.3 查看设备列表

登录成功后客户端自动请求摄像头列表。

演示操作：

1. 查看右侧“设备列表”。
2. 展开摄像头节点。
3. 查看“主码流”“子码流”等通道。
4. 在搜索框输入摄像头名称关键字，过滤设备。
5. 点击搜索按钮，重新刷新设备列表。

### 6.4 实时视频监控

演示操作：

1. 双击设备列表中的某一路通道。
2. 当前选中的视频窗口开始连接流。
3. 也可以右键设备通道，选择“观看”，把视频放到指定窗口。
4. 点击底部 `1 / 2 / 4 / 8 / 16` 切换分屏模式。
5. 右键视频窗口，可对当前窗口执行停止播放等操作。

客户端实时流播放链路：

```text
Qt 客户端 -> TLV START_STREAM -> server -> RTSP 摄像头 -> TLV STREAM_METADATA / STREAM_PACKET -> Qt 解码显示
```

### 6.5 云台控制

演示操作：

1. 在设备列表中选择支持云台的摄像头，例如 `camera_192_168_6_100`。
2. 先双击该通道打开视频，再在视频窗口右键选择“设为云台控制对象”。
3. 观察云台状态栏，确认“观看”和“控制”指向同一个摄像头通道。
4. 点击云台方向盘的上、下、左、右、斜向或中间归位按钮。
5. 使用“变倍”“聚焦”“光圈”的 `+ / -` 按钮。
6. 调整“延迟”滑块，改变单次云台动作持续时间。
7. 如果多个客户端同时申请同一路云台，客户端会进入排队状态，并显示前面等待人数。

服务端云台链路：

```text
客户端申请控制权 -> 服务端检查摄像头和控制队列 -> 返回 granted 或 busy
客户端发送 PTZ_CONTROL -> 服务端转换为摄像头 HTTP 请求 -> 返回执行结果
```

如果另一个客户端已经占用控制权，当前客户端会收到忙碌提示。GUI 不直接丢弃操作，而是保留最后一次待执行动作，定时重新申请控制权，拿到 `granted` 后自动发送该动作。

客户端云台控制权设计：

| 设计点 | 说明 |
| --- | --- |
| 控制对象必须来自已打开视频 | 只有视频框正在播放的摄像头通道才能设为云台控制对象，避免对不可见通道误操作 |
| 观看对象和控制对象分离展示 | 状态栏显示 `观看: 摄像头/通道 | 控制: 摄像头/通道`，排队时显示 `排队中` 和前方人数 |
| 单控制对象策略 | 切换到新摄像头前会释放旧控制权；关闭正在控制的视频窗口时也会释放控制权 |
| 排队重试 | 收到 `PTZ_BUSY` 后进入等待状态，每 `2000ms` 重新发送 `PTZ_ACQUIRE` |
| 失败清理 | 断开连接、退出登录、云台失败或窗口关闭时清空控制对象、等待对象和待执行动作 |
| 主动取消 | 右键视频窗口选择“解除控制”时发送 `PTZ_RELEASE` 并清理本地状态 |
| 待执行动作 | 用户排队期间点击的最后一次云台动作会保存为待执行动作，获得控制权后立即补发 |
| 动作速度 | GUI 统一使用速度 `5` 发送云台动作 |
| 动作时长 | “延迟”滑块默认 `200ms`；左右和变倍按滑块值执行，上下方向按 `1/3` 缩短，斜向动作按 `1/sqrt(2)` 后再按 `1/3` 缩短 |

客户端涉及的 TLV 消息：

| 消息 | 用途 |
| --- | --- |
| `PTZ_ACQUIRE` | 申请指定摄像头通道的云台控制权 |
| `PTZ_RELEASE` | 释放已持有或正在排队的控制权 |
| `PTZ_CONTROL` | 发送方向、变倍、归位、停止等具体云台动作 |
| `PTZ_RESULT` | 返回动作执行成功或控制权申请成功，申请成功时消息内容为 `granted` |
| `PTZ_BUSY` | 表示控制权被占用；消息中如果包含 `ahead=N`，客户端会解析并显示前方等待人数 |

### 6.6 录像配置

演示操作：

1. 在右侧底部切换到“设备控制”。
2. 点击“刷新摄像头配置”。
3. 查看摄像头、通道和录像开关。
4. 切换支持录像通道的开关。

当前客户端界面只允许主码流相关通道操作录像开关，非支持通道会禁用。

### 6.7 视频回放

演示操作：

1. 点击顶部“视频回放”。
2. 选择摄像头和通道。
3. 选择日期或时间范围。
4. 点击查询，获取录像分片列表。
5. 选择录像分片后开始回放。
6. 可停止回放或导出合并后的录像文件。

录像链路：

```text
server 自动录制 RTSP -> TS 分片写入 data/records -> record_segments 写入 MySQL
Qt 查询录像列表 -> TLV RECORD_LIST
Qt 请求回放 -> TLV RECORD_METADATA / RECORD_CHUNK -> 本地播放
```

## 7. 命令行演示与验证

如果现场不方便启动 Qt 客户端，可以使用服务端自带 Python 调试工具验证核心 TLV 能力。

### 7.1 查看摄像头列表

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 cameras
```

### 7.2 测试实时流转发

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 stream camera_192_168_6_100 --channel 0 --messages 5
```

期望能收到：

- `STREAM_METADATA`
- `STREAM_PACKET`

### 7.3 测试云台移动

向上转动 500 毫秒：

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 move camera_192_168_6_100 up --channel 0 --speed 1 --duration-ms 500
```

紧急停止：

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 stop camera_192_168_6_100 --channel 0
```

### 7.4 测试云台控制权

占用控制权：

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 acquire camera_192_168_6_100 --channel 0
```

释放控制权：

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 release camera_192_168_6_100 --channel 0
```

### 7.5 查看录像配置

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 record-config
```

### 7.6 开启或关闭录像

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 set-record camera_192_168_6_100 --channel 0 --enabled
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 set-record camera_192_168_6_100 --channel 0 --no-enabled
```

### 7.7 查询录像分片

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 records camera_192_168_6_100 --channel 0 --start-ms 1760000000000 --end-ms 1760000060000
```

### 7.8 测试录像回放数据

```bash
cd server
python3 scripts/ptz_tool.py --host 127.0.0.1 --port 8123 playback camera_192_168_6_100 --channel 0 --start-ms 1760000000000 --end-ms 1760000060000 --messages 5
```

期望能收到：

- `RECORD_METADATA`
- `RECORD_CHUNK`

## 8. 演示话术建议

可以按下面顺序演示：

1. “这是一个 C++ 服务端加 Qt 桌面客户端的视频监控平台，通信协议是自定义 TLV。”
2. “先启动服务端，服务端会连接 MySQL、加载摄像头配置，并自动启动录像任务。”
3. “客户端登录后会自动拉取设备列表，右侧展示摄像头和通道。”
4. “双击通道即可把实时流放入当前分屏，底部可以切换 1 到 16 路布局。”
5. “云台控制前客户端会先申请控制权，服务端能处理多客户端冲突。”
6. “录像功能由服务端自动分片保存，客户端可以查询指定时间段并回放。”
7. “系统设置页可以查看当前连接状态，并支持断开和重新登录。”

## 9. 常见问题

### 9.1 客户端登录失败

检查：

1. 服务端是否已启动。
2. 客户端连接 IP 是否为服务端 IP。
3. 服务端端口是否为 `8123`。
4. MySQL 是否已初始化 `monitor` 数据库。
5. 登录密码是否为 `1234`，不是登录框默认的 `admin123`。

### 9.2 设备列表为空

检查：

1. `conf/schema.sql` 是否已经执行。
2. MySQL `monitor.cameras` 表是否有数据。
3. 服务端启动日志是否有 `camera registry loaded`。

### 9.3 视频无法播放

检查：

1. RTSP 摄像头地址是否能从服务端机器访问。
2. 摄像头账号密码是否正确。
3. 客户端是否启用 FFmpeg 解码能力。
4. 服务端日志中是否有 RTSP 或流转发错误。

### 9.4 云台控制失败

检查：

1. 摄像头是否配置了 `ptz_request_template`。
2. 当前摄像头是否被其他客户端占用控制权。
3. 摄像头 HTTP 云台接口是否能从服务端访问。

### 9.5 没有录像数据

检查：

1. 摄像头 `record_enabled` 是否为 `1`。
2. 服务端是否能访问 RTSP 流。
3. `server/data/records` 是否有生成 TS 文件。
4. MySQL `record_segments` 表是否有索引记录。

## 10. 代码入口参考

| 模块 | 文件 |
| --- | --- |
| 服务端启动入口 | `server/src/main.cpp` |
| 服务端配置 | `server/conf/server.conf` |
| 数据库初始化 | `server/conf/schema.sql` |
| TLV 类型定义 | `server/include/TLV/TLVTypes.h` |
| 业务分发 | `server/src/BusinessService.cpp` |
| TCP 服务与任务分发 | `server/src/MonitorServer.cpp` |
| 实时流转发 | `server/src/MediaStreamForwarder.cpp` |
| 录像管理 | `server/src/RecordManager.cpp` |
| 客户端启动入口 | `monitor-client/main.cpp` |
| 客户端主窗口 | `monitor-client/mainwindow.cpp` |
| 客户端 API 封装 | `monitor-client/api/MonitorApiClient.cpp` |
| 客户端 TLV 通信 | `monitor-client/api/TlvClient.cpp` |
| 视频窗口 | `monitor-client/video/VideoCellWidget.cpp` |
| 录像回放 | `monitor-client/widgets/RecordPlaybackWidget.cpp` |
| 录像配置 | `monitor-client/widgets/RecordConfigWidget.cpp` |
| 系统设置 | `monitor-client/widgets/SystemSettingsWidget.cpp` |
