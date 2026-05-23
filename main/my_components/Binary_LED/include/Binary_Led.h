#pragma once
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

typedef enum{
    BL_LIGHT_OFF,
    BL_LIGHT_ON
}b_led_light_e;

typedef enum{
    BL_OFF,
    BL_ON,
    BL_BLINK_ON,
    BL_BLINK_OFF
}b_led_state_e;

typedef struct{
    
    gpio_num_t pin;
    b_led_light_e light_mode;
    int level;
    int milisecOn;
    int milisecOff;
    bool ledON;
    b_led_state_e current_state;
    b_led_state_e next_state;
    b_led_state_e previous_state;
    TickType_t tick_elapse;
    
}b_led_s;

typedef b_led_s* b_led_handle;

b_led_handle b_led_init(int pin,b_led_light_e mode, bool blink);

void b_led_update(b_led_handle led);

void b_led_setled(b_led_handle led, b_led_light_e light_mode);

void b_led_blink(b_led_handle led, int milisecOn, int milisecOff);

void b_led_destroy(b_led_handle *led);
