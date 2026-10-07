set(src_dirs
            LED
            KEY
            LCD
            QMI
            WIFI)

set(include_dirs
            LED
            KEY
            LCD
            QMI
            WIFI)
            
set(requires
            esp_wifi
            nvs_flash
            esp_adc
            esp_lcd
            esp_lcd_touch
            espressif__esp_lcd_touch_ft5x06
            esp_lvgl_port
            lvgl
            driver
            )
            
idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)