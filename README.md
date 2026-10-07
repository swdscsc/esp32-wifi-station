# ESP32-S3 WiFi 站模式（STA）连接与事件处理

在 ESP32-S3 上实现 WiFi Station 模式联网：使用 FreeRTOS **事件组（Event Group）**
在 WiFi 连接成功时通知应用任务，并提供 SoftAP 模式的对照实现。

---

## 硬件与框架

| 项目 | 参数 |
| --- | --- |
| MCU | ESP32-S3 |
| 框架 | ESP-IDF + FreeRTOS |
| WiFi 模式 | STA（连热点）/ AP（板子做热点，可选） |
| 加密方式 | WPA2-PSK |

## 两种模式

| 模式 | 文件 | 说明 |
| --- | --- | --- |
| **STA** | `components/WIFI/wifi_sta.c` | 板子连接外部热点，本仓库默认使用 |
| AP | `components/WIFI/wifi_ap.c` | 板子自身作为热点，最多支持 4 个设备接入 |

## 核心实现：用事件组做连接同步

`main/main.c` 中并没有用轮询去查 WiFi 状态，而是注册回调 + 事件组：

```c
#define WIFI_CONNECTED_BIT  BIT0
static EventGroupHandle_t s_wifi_ev = NULL;

// WiFi 组件在连接成功时回调应用
void wifi_event_handler(WIFI_EV_e ev)
{
    if (ev == WIFI_CONNECTED) {
        xEventGroupSetBits(s_wifi_ev, WIFI_CONNECTED_BIT);
    }
}

void app_main(void)
{
    nvs_flash_init();
    wifi_sta_init(wifi_event_handler);   // 传入回调
    ...
}
```

这样做的好处是应用任务可以 `xEventGroupWaitBits()` 阻塞等待联网完成，
而不是死等固定延时 —— 联网快就早跑，联网慢也不会误判。

### 底层事件注册

`wifi_sta.c` 中注册了两类系统事件：

```c
esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
esp_event_handler_register(IP_EVENT,   IP_EVENT_STA_GOT_IP, &event_handler, NULL);
```

- `WIFI_EVENT`：处理断开/重连，掉线后自动调用 `esp_wifi_connect()` 重连
- `IP_EVENT_STA_GOT_IP`：拿到 IP 后才算真正联网成功

> **注意**：只监听 `WIFI_EVENT` 的 `CONNECTED` 是不够的，必须等到 `GOT_IP`
> 才能开始网络通信，这是初学 WiFi 开发时常见的坑。

## 编译前必做：配置 WiFi 账号密码

**本仓库不包含任何真实账号密码。** 请先创建私密配置文件：

```bash
cp components/WIFI/wifi_secrets_example.h components/WIFI/wifi_secrets.h
```

然后编辑 `wifi_secrets.h` 填入你自己的信息：

```c
#define  DEFAULT_WIFI_SSID       "你的热点名"
#define  DEFAULT_WIFI_PASSWORD   "你的热点密码"
```

`wifi_secrets.h` 已被 `.gitignore` 忽略，**不会被提交到仓库**。

## 编译与烧录

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

串口会打印 `wifi的sta模式初始化完成`，拿到 IP 后事件组置位。

## 后续优化方向

- [ ] 接入 MQTT，把传感器数据上报到华为云 / EMQX
- [ ] 增加断线重连的退避策略（避免频繁重连）
- [ ] 用 `nvs` 保存账号密码，支持配网（SmartConfig / BLE 配网）

---

## License

MIT
