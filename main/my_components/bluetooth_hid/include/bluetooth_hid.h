#pragma once
#include <stdint.h>
#include "esp_err.h"

typedef enum {
    BLE_STATE_DISCONNECTED = 0,
    BLE_STATE_ADVERTISING,
    BLE_STATE_CONNECTED
} ble_state_t;

static ble_state_t ble_state = BLE_STATE_DISCONNECTED;
ble_state_t ble_hid_get_state(void);

void ble_hid_init(void);
void ble_hid_send_buttons(uint8_t buttons);
void ble_hid_start_advertising(void);
bool disconnect_or_advertise(void);