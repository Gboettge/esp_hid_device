idf_component_register(SRCS "bluetooth_hid.c"
                    INCLUDE_DIRS "include"
                    REQUIRES bt esp_hid esp_driver_gpio)
