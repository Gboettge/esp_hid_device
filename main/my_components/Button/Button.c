#include "Button.h"



button_handle button_init(int pin, strapping_mode mode){

    button_handle new_button = pvPortMalloc (sizeof(button));
    new_button->pin = pin;
    new_button->mode = mode;
    new_button->latch = 0;
    new_button->pressed = false;
    new_button->onPressed = NULL;
    new_button->onReleased = NULL;
    gpio_input_enable(pin);
    gpio_intr_disable(pin);

    new_button->current_state = STATE_OFF;
    new_button->next_state = STATE_OFF;
    new_button->previous_state = STATE_OFF;
    new_button->level = 0;
    new_button->debounce_tick = 0;
    
    if(mode == GPIO_PULLUP){
        gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    }
    else if(mode == GPIO_PULLDOWN){
        gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
    }
    return new_button;
}

//debounce (latch?)
// 10ms max per uppdatering
void button_update(button_handle button)
{
    TickType_t current_tick = xTaskGetTickCount();
    if (button->mode == GPIO_PULLDOWN)
        button->level = gpio_get_level(button->pin); // tar inte bort debounce
    if (button->mode == GPIO_PULLUP)
        button->level = !gpio_get_level(button->pin);
    
    switch (button->current_state)
    {

    case STATE_ON:
        button->pressed = 1;
        if (button->level == 0)
        {
            button->next_state = DEBOUNCE_STATE_ON;
            button->debounce_tick = current_tick;
            
        }
        break;
    case DEBOUNCE_STATE_ON:
        button->pressed = 0;
        if (current_tick - button->debounce_tick >= pdMS_TO_TICKS(30))
        {
            button->next_state = STATE_OFF;
            if (button->onReleased != NULL)
            {
                button->onReleased(button->pin, button->onReleased_arg);
            }
            else {
                printf("Non function button: %d released \n", button->pin);
            }
        }
        break;
    case STATE_OFF:
        button->pressed = 0;
        if (button->level == 1)
        {
            button->next_state = DEBOUNCE_STATE_OFF;
            button->debounce_tick = current_tick;
        }
        break;
    case DEBOUNCE_STATE_OFF:
        button->pressed = 1;
        if (current_tick - button->debounce_tick >= pdMS_TO_TICKS(30))
        {
            button->next_state = STATE_ON;
            if (button->onPressed != NULL)
            {
                button->onPressed(button->pin, button->onPressed_arg);
            }
            else {
                printf("Non function button: %d Pressed \n", button->pin);
            }
        }
        break;
    default:
        printf("error %d\n", button->current_state);
        break;
    }
    // if(button->next_state != button->current_state){
    //     printf("%d -> %d\n", button->current_state, button->next_state);
    // }
    button->previous_state = button->current_state;
    button->current_state = button->next_state;
}
   
bool button_isPressed (button_handle button){
    return button->pressed;
}

void button_setOnReleased (button_handle button, onbutton_f onReleased, void* arg){
    button->onReleased_arg = arg;
    button->onReleased = onReleased;
}
void button_setOnPressed (button_handle button, onbutton_f onPressed, void* arg){
    button->onPressed_arg = arg;
    button->onPressed = onPressed;
}

void button_print_press_num (int pin){
    printf ("Button: %d pressed.\n", pin);
}

void button_print_num_x_ten(int pin){
    printf ("Button: %d x 10 = %d\n", pin, pin * 10);
}

void button_print_released(int pin){
    printf ("Button: %d released\n", pin);
}


void button_destroy(button_handle *button){
    vPortFree(button);
}


// int init_button(pin, int x);

// int update(xxx);

// bool isPressed (int xxx);

// void setonReleased ( void(*onReleased)(int pin));
/*
moving average filter (mjukvaru filter)
lista
index
adc värde
*/


/*
void button_update(button_handle button)
{
    TickType_t startTick = xTaskGetTickCount();

    if (button->mode == GPIO_PULLDOWN)
        button->pressed = gpio_get_level(button->pin); // tar inte bort debounce
    if (button->mode == GPIO_PULLUP)
    {
        button->pressed = !gpio_get_level(button->pin);
        // button->pressed = false;
        // button->latch = 1;
    }

    if (button->pressed && !button->latch)
    {
        TickType_t tickDifference = xTaskGetTickCount() - startTick;
        if (tickDifference >= pdMS_TO_TICKS(200))
        {
            button->latch = 1;
            button->pressed = true;
        }

        if (button->onReleased != NULL)
            button->onReleased(button->pin);
        else
            printf("pressed %d\n", button->pressed);
        return;
    }
    else if (!button->pressed && button->latch)
    {
        TickType_t tickDifference = xTaskGetTickCount() - startTick;

        if (tickDifference >= pdMS_TO_TICKS(200))
        {
        }
        button->latch = 0;
        button->pressed = false;
        //  printf("ticks passed: %ld\n", tickDifference);
        // printf("not pressed %d\n", button->pressed);
        return;
    }
    // ingen vtask delay här
    vTaskDelay(1);
    // printf("ticks passed: %ld\n", startTick);
    */

//ordna ifall man nyttjar pullup istäället för pullup