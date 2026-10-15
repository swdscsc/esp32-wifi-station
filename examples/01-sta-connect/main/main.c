
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
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

// 示例 01：ESP32 作为 STA 连接热点，事件组等待拿到 IP 后打印
void app_main(void)
{
    wifi_connect();
    printf("WiFi connected, IP obtained!\n");
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
}
