#pragma once

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

typedef enum {
    GPIO_PULLUP,
    GPIO_PULLDOWN 
}strapping_mode;

typedef enum{
    STATE_ON,
    DEBOUNCE_STATE_ON,
    STATE_OFF,
    DEBOUNCE_STATE_OFF
}state_e;
typedef void(*onbutton_f)(int, void*);

typedef struct {
    int pin;
    //state_e button_state;
    strapping_mode mode;
    int latch;
    bool pressed;
    onbutton_f onPressed;
    onbutton_f onReleased;
    void* onPressed_arg;
    void* onReleased_arg;
    state_e current_state;
    state_e next_state;
    state_e previous_state;
    int level;
    TickType_t debounce_tick;
} button;

typedef button* button_handle;

button_handle button_init(int pin, strapping_mode pull);

void button_update(button_handle button);

bool button_isPressed (button_handle button);

void button_setOnReleased (button_handle button, onbutton_f onReleased, void* arg);

void button_setOnPressed (button_handle button, onbutton_f onPressed, void* arg);

void button_print_press_num (int pin);

void button_print_num_x_ten(int pin);

void button_print_released(int pin);

void button_destroy(button_handle *button);