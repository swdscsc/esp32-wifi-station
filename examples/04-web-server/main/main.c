
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
#include "esp_http_server.h"
#include "esp_log.h"

// 示例 04：板子启动一个 HTTP 服务器，访问 / 返回页面
static esp_err_t root_handler(httpd_req_t *req)
{
    const char *html = "<h1>ESP32 Web Server</h1><p>Hello from ESP32</p>";
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void app_main(void)
{
    wifi_connect();
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    httpd_start(&server, &cfg);
    httpd_uri_t uri = { .uri = "/", .method = HTTP_GET,
                        .handler = root_handler, .user_ctx = NULL };
    httpd_register_uri_handler(server, &uri);
    ESP_LOGI("web", "server started, open http://<board-ip>/");
}
