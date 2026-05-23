#include <stdio.h>
#include "Binary_led.h"

b_led_handle b_led_init(int pin, b_led_light_e light_mode, bool blink){
    b_led_handle newled = pvPortMalloc (sizeof(b_led_s));
   
    
    newled->pin = pin;
    newled->level = 0;
    newled->milisecOn = 0;
    newled->milisecOff = 0;
    newled->light_mode = light_mode;
    newled->current_state = BL_ON;
    newled->ledON = blink;
    newled->tick_elapse = 0;

    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    return newled;     
}

void b_led_update(b_led_handle led)
{
    TickType_t current_tick = xTaskGetTickCount();
    switch (led->current_state)
    {
    case BL_ON:
        if(led->light_mode == BL_LIGHT_ON){
            gpio_set_level(led->pin, BL_LIGHT_ON);
        }
        led->next_state = BL_OFF;
        //led->light_mode = LIGHT_ON;
        break;

    case BL_OFF:
        if(led->light_mode == BL_LIGHT_OFF){
            gpio_set_level(led->pin, BL_LIGHT_OFF);
        }
        led->next_state = BL_ON;
        //led->light_mode = LIGHT_OFF;
        if(led->ledON == true){
            //printf("ledON == true\n");
            led->next_state = BL_BLINK_ON;
        }
        break;

    case BL_BLINK_ON:
        
        if(current_tick - led->tick_elapse >= pdMS_TO_TICKS(led->milisecOn))
        {
            //printf("\n\n");
            led->next_state = BL_BLINK_OFF;
            led->tick_elapse = current_tick;
            //led->tick_elapse = 0;
        }
        else{
            led->next_state = BL_BLINK_ON;
            
        }
        
        gpio_set_level(led->pin, 1);
        break;
    case BL_BLINK_OFF:
        if(current_tick - led->tick_elapse >= pdMS_TO_TICKS(led->milisecOff))
        {
        //printf("\n\n");
        led->next_state = BL_BLINK_ON;
        led->tick_elapse = current_tick;
        //led->tick_elapse = 0;
        }
        else{
            led->next_state = BL_BLINK_OFF;
        }
        gpio_set_level(led->pin, 0);
        break;
    default:
        printf("error %d\n", led->current_state);
        break;
    }
    // if(led->next_state != led->current_state){
    //      printf("%d -> %d\n", led->current_state, led->next_state);
    // }
    led->previous_state = led->current_state;
    led->current_state = led->next_state;
}

void b_led_setled(b_led_handle led, b_led_light_e light_mode){
    
    led->light_mode = light_mode;
    if(light_mode == BL_LIGHT_ON){
        led->current_state = BL_ON;
        
    }
    else{
        led->current_state = BL_OFF;
    }
}

void b_led_blink(b_led_handle led, int milisecOn, int milisecOff){
    led->current_state = BL_BLINK_ON;
    led->milisecOn = milisecOn;
    led->milisecOff = milisecOff;
}

void b_led_destroy(b_led_handle *led){
    vPortFree(led);
}