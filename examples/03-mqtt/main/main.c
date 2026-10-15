
// 连接 WiFi（事件组等待拿到 IP），需在 app_main 开头调用
#define EX_SSID  "YOUR_SSID"
#define EX_PASS  "YOUR_PASS"
#define EX_BIT   BIT0
static EventGroupHandle_t s_ev;
static void on_got_ip(void* arg, esp_event_base_t base, int32_t id, void* data)
{
    xEventGroupSetBits(s_ev, EX_BIT);
}
static void wifi_connect(void)
{
    s_ev = xEventGroupCreate();
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, NULL);
    wifi_config_t wc = { .sta = { .ssid = EX_SSID, .password = EX_PASS } };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wc);
    esp_wifi_start();
    esp_wifi_connect();
    xEventGroupWaitBits(s_ev, EX_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
}

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "esp_log.h"

// 示例 03：MQTT 发布/订阅（公共 broker）
static esp_mqtt_client_handle_t s_client;
static esp_err_t on_event(esp_mqtt_event_handle_t e)
{
    switch (e->event_id) {
        case MQTT_EVENT_CONNECTED:
            esp_mqtt_client_subscribe(s_client, "swd/test", 0);
            break;
        case MQTT_EVENT_DATA:
            printf("%.*s: %.*s\n", e->topic_len, e->topic, e->data_len, e->data);
            break;
        default: break;
    }
    return ESP_OK;
}

void app_main(void)
{
    wifi_connect();
    esp_mqtt_client_config_t cfg = { .broker.url = "mqtt://broker.emqx.io" };
    s_client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(s_client, MQTT_EVENT_ANY, on_event, s_client);
    esp_mqtt_client_start(s_client);

    int n = 0;
    char buf[32];
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        snprintf(buf, sizeof(buf), "hello %d", n++);
        esp_mqtt_client_publish(s_client, "swd/test", buf, 0, 0, 0);
    }
}
