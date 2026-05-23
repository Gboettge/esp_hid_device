#include "bluetooth_hid.h"
#include "Binary_LED.h"
#include "Button.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"

// static const char *TAG = "main";
#define DEBOUNCE_MS 50

// typedef enum {
    //     BLE_STATE_DISCONNECTED = 0,
    //     BLE_STATE_ADVERTISING,
    //     BLE_STATE_CONNECTED

// } ble_state_t;

// static ble_state_t ble_state = BLE_STATE_DISCONNECTED;
// int previous_state;
// int current_state;

button_handle button1;
button_handle button2;
button_handle button3;
b_led_handle status_LED;

void onPress1(int pin, void *arg);
void onPress2(int pin, void *arg);
void onPress3(int pin, void *arg);
void onRelease(int pin, void *arg);

// ble_state_t current_state = BLE_STATE_ADVERTISING;
ble_state_t previous_state = BLE_STATE_CONNECTED;
uint8_t package_two = 0x02; //b
uint8_t package_one = 0x01; //a
uint8_t package_empty = 0x00;
// static int read_buttons(void) {
    //   int b1 = gpio_get_level(BTN1_GPIO); /* active low */
    //   int b2 = gpio_get_level(BTN2_GPIO);
//   return (b1 ? 0x01 : 0) | (b2 ? 0x02 : 0);
// }
void app_main(void)
{
    printf("hello\n");
    button1 = button_init(15, GPIO_PULLUP);
    // clear_recieved_data();
    button2 = button_init(6, GPIO_PULLUP);
    button3 = button_init(22, GPIO_PULLUP);
    status_LED = b_led_init(4, BL_LIGHT_OFF, false);
    esp_err_t ret = nvs_flash_init();
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            
            ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    button_setOnPressed(button1, (onbutton_f)onPress1, status_LED);
    button_setOnPressed(button2, (onbutton_f)onPress2, status_LED);
    button_setOnPressed(button3, (onbutton_f)onPress3, status_LED);
    button_setOnReleased(button1, (onbutton_f)onRelease, status_LED);
    button_setOnReleased(button2, (onbutton_f)onRelease, status_LED);

    ESP_ERROR_CHECK(ret);
    // ble_state_t previous_state = ble_hid_get_state();
    ble_hid_init();
    
    while (1) {
        button_update(button1);
        button_update(button2);
        button_update(button3);
        b_led_update(status_LED);
        vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
        // ble_state = ble_hid_get_state();
        ble_state_t current_state = ble_hid_get_state();

        if (current_state != previous_state){
            switch(current_state)
            {
            case BLE_STATE_DISCONNECTED:
                printf("DISCONNECTED\n");
                b_led_setled(status_LED, BL_LIGHT_OFF);
                previous_state = current_state;
                break;
                
                case BLE_STATE_ADVERTISING:
                printf("ADVERTISING\n");
                previous_state = current_state;
                b_led_blink(status_LED, 300, 300);
                break;
                
                case BLE_STATE_CONNECTED:
                printf("CONNECTED\n");
                b_led_setled(status_LED, BL_LIGHT_ON);
                previous_state = current_state;
                break;
            }
        }
    }
}

// ble_hid_set_pin_gpio(BTN1_GPIO, BTN2_GPIO);
void onPress1(int pin, void *arg) { 
    // b_led_handle b = (b_led_handle)arg; b_led_setled(b, BL_LIGHT_ON);
    ble_hid_send_buttons(package_one);
}
void onPress2(int pin, void *arg){
    // b_led_handle b = (b_led_handle)arg; b_led_setled(b, BL_LIGHT_ON);
    ble_hid_send_buttons(package_two);
}
void onPress3(int pin, void *arg){
    // b_led_handle b = (b_led_handle)arg; b_led_blink(b, 600, 600);
    bool reboot = disconnect_or_advertise();
    if (reboot){
        nvs_flash_erase();
        ble_hid_init();
        previous_state = BLE_STATE_DISCONNECTED;
        esp_restart();

    }
    else{
        return;                                                                
    }
}

void onRelease(int pin, void *arg){
    // b_led_handle b = (b_led_handle)arg; b_led_setled(b, BL_LIGHT_OFF);
    ble_hid_send_buttons(package_empty);
}

