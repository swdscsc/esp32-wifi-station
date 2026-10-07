#include <stdio.h>
#include "freertos/FreeRTOS.h"            //系统
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "qmi.h"
#include "pic.h"
#include "wifi_ap.h"
#include "wifi_sta.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

#define  WIFI_CONNECTED_BIT  BIT0
static EventGroupHandle_t s_wifi_ev = NULL;

//wifi通知事件
void wifi_event_handler(WIFI_EV_e ev)
{
    if(ev == WIFI_CONNECTED)
    {
        xEventGroupSetBits(s_wifi_ev,WIFI_CONNECTED_BIT);
    }
}


void app_main(void)
{
    led_init();
    key_init();
    bsp_lvgl_start();                                              //lcd+lvgl配置  
    IIC_Init();                                                    //IIC配置函数
    Qmi8658_Init();                                                //姿态传感器配置
    //wifi_init_softap();                                            //wifi的ap模式配置
    nvs_flash_init();
    wifi_sta_init(wifi_event_handler);

    while (1)    
    { 
       vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

