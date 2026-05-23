idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES 
                       Button 
                       Binary_LED
                       bluetooth_hid
                       bt
                       nvs_flash
                       )