
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
#include "esp_http_client.h"
#include "esp_log.h"

// 示例 02：联网后通过 HTTPS 抓取网页内容
static esp_err_t on_data(esp_http_client_event_t *ev)
{
    if (ev->event_id == HTTP_EVENT_ON_DATA && ev->data_len > 0)
        printf("%.*s", ev->data_len, (char *)ev->data);
    return ESP_OK;
}

void app_main(void)
{
    wifi_connect();
    esp_http_client_config_t cfg = {
        .url = "https://example.com",
        .event_handler = on_data,
    };
    esp_http_client_handle_t c = esp_http_client_init(&cfg);
    esp_http_client_perform(c);
    esp_http_client_cleanup(c);
}
