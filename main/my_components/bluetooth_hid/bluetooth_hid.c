#include "bluetooth_hid.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_att.h"
#include "host/ble_hs_mbuf.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


static const char *TAG = "ble_hid";
static uint16_t conn_handle = BLE_HS_CONN_HANDLE_NONE;
static uint16_t report_chr_handle = 0;

// static const uint8_t report_map[] = {  //Gamepad mapping
//   0x05,0x01,0x09,0x05,0xA1,0x01,
//   0x05,0x09,0x19,0x01,0x29,0x02,
//   0x15,0x00,0x25,0x01,0x95,0x02,
//   0x75,0x01,0x81,0x02,0x95,0x01,
//   0x75,0x06,0x81,0x03,0xC0
// };

static const uint8_t report_map[] = { //Keyboard mapping
    0x05, 0x01,
    0x09, 0x06,
    0xA1, 0x01,

    0x05, 0x07,
    0x19, 0xE0,
    0x29, 0xE7,
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, 0x08,
    0x81, 0x02,

    0x95, 0x01,
    0x75, 0x08,
    0x81, 0x01,

    0x95, 0x06,
    0x75, 0x08,
    0x15, 0x00,
    0x25, 0x65,
    0x05, 0x07,
    0x19, 0x00,
    0x29, 0x65,
    0x81, 0x00,

    0xC0
};



//
static int hid_access_cb(uint16_t conn_handle,
                         uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt,
                         void *arg)
{
    const ble_uuid_t *uuid = ctxt->chr->uuid;

    /* HID Report Map */
    if (ble_uuid_u16(uuid) == 0x2A4B) {

        os_mbuf_append(ctxt->om,
                       report_map,
                       sizeof(report_map));

        return 0;
    }

    /* HID Info */
    if (ble_uuid_u16(uuid) == 0x2A4A) {

        uint8_t hid_info[4] = {
            0x11, 0x01,
            0x00,
            0x02
        };

        os_mbuf_append(ctxt->om,
                       hid_info,
                       sizeof(hid_info));

        return 0;
    }

    /* Input Report */
    if (ble_uuid_u16(uuid) == 0x2A4D) {

        uint8_t empty_report[1] = {0};

        os_mbuf_append(ctxt->om,
                       empty_report,
                       sizeof(empty_report));

        return 0;
    }
    if (ble_uuid_u16(uuid) == 0x2908) {

        uint8_t report_ref[2] = {
            0x01, /* Report ID */
            0x01  /* Input Report */
        };

        os_mbuf_append(ctxt->om,
                    report_ref,
                    sizeof(report_ref));

        return 0;
    }

    return BLE_ATT_ERR_UNLIKELY;
}
//


static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1812),

        .characteristics = (struct ble_gatt_chr_def[]) {

            {
                .uuid = BLE_UUID16_DECLARE(0x2A4A),
                .access_cb = hid_access_cb,
                .flags = BLE_GATT_CHR_F_READ,
            },

            {
                .uuid = BLE_UUID16_DECLARE(0x2A4B),
                .access_cb = hid_access_cb,
                .flags = BLE_GATT_CHR_F_READ,
            },

            {
                .uuid = BLE_UUID16_DECLARE(0x2A4D),
                .access_cb = hid_access_cb,
                .val_handle = &report_chr_handle,

                .flags = BLE_GATT_CHR_F_READ |
                         BLE_GATT_CHR_F_NOTIFY,

                .descriptors = (struct ble_gatt_dsc_def[]) {
                    {
                        .uuid = BLE_UUID16_DECLARE(0x2908),
                        .att_flags = BLE_ATT_F_READ,
                        .access_cb = hid_access_cb,
                    },
                    {
                        0
                    }
                },
            },

            {
                0
            }
        }
    },

    {
        0
    }
};

static int ble_app_gap_event(struct ble_gap_event *event, void *arg) {
  switch (event->type) {
  
    case BLE_GAP_EVENT_SUBSCRIBE:
    ESP_LOGI(TAG,
             "Client subscribed! attr=%d notify=%d",
             event->subscribe.attr_handle,
             event->subscribe.cur_notify);
    break;

    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
        conn_handle = event->connect.conn_handle;

        ble_state = BLE_STATE_CONNECTED;

        ESP_LOGI(TAG, "Connected handle=%d", conn_handle);
        } else {
        ESP_LOGI(TAG, "Connect failed: %d", event->connect.status);
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Disconnected reason=%d", event->disconnect.reason);
        conn_handle = BLE_HS_CONN_HANDLE_NONE;
        
        ble_state = BLE_STATE_DISCONNECTED;
                                                                                                                                        
        break;
    default:
        break;
    }
  return 0;
}

void ble_hid_start_advertising(void) {
    ESP_LOGI(TAG, "Starting advertising...");

    ble_state = BLE_STATE_ADVERTISING;

    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    fields.name =(uint8_t *)"ESP32-HID";
    fields.name_len = strlen("ESP32-HID");
    fields.name_is_complete = 1;

    // uint16_t appearance = 0x03C0; /* Gamepad */
    uint16_t appearance = 0x03C1; /* Keyboard */
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = 0;
    fields.appearance_is_present = 1;
    fields.appearance = appearance;

    /* advertise HID service UUID (0x1812) */
    static const ble_uuid16_t uuid16[] = { BLE_UUID16_INIT(0x1812) };
    fields.uuids16 = uuid16;
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params;
    memset(&adv_params,0,sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    int rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params, ble_app_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gap_adv_start failed: %d", rc);
        return;
    }
  
    ESP_LOGI(TAG, "Advertising started successfully");
}
    
void ble_hid_send_buttons(uint8_t buttons)
{
    ESP_LOGI(TAG, "ble_hid_send_buttons called");

    if (conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGE(TAG, "No active connection");
        return;
    }

    if (report_chr_handle == 0) {
        ESP_LOGE(TAG, "report_chr_handle is 0");
        return;
    }

    uint8_t report[8] = {0};
        
    if (buttons == 0x01) {
        report[2] = 0x04; // 'a'
    }
    else if (buttons == 0x02) {
        report[2] = 0x05; // 'b'
    }
    else {
        report[2] = 0x00; // release
    }
   
    
    ESP_LOGI(TAG,
             "Sending HID report: 0x%02X on conn=%d handle=%d",
             report[2],
             conn_handle,
             report_chr_handle);

    struct os_mbuf *om =
        ble_hs_mbuf_from_flat(report, sizeof(report));

    if (om == NULL) {
        ESP_LOGE(TAG, "Failed to allocate mbuf");
        return;
    }

    int rc = ble_gatts_notify_custom(conn_handle, report_chr_handle, om);
    // int rc = ble_att_svr_write_local(report_chr_handle, om); //Update locally

    if (rc != 0) {
        ESP_LOGE(TAG,
                 "notify failed rc=%d",
                 rc);
        return;
    }

    // ESP_LOGI(TAG, "Local write success");
    ESP_LOGI(TAG, "Notification sent");

    ble_gatts_chr_updated(report_chr_handle);

    ESP_LOGI(TAG, "Notification triggered");
}

static void ble_on_sync(void) {
  ESP_LOGI(TAG, "ble_on_sync called!");
  
//   ble_svc_gap_init();
//   ble_svc_gatt_init();
//   ble_gatts_count_cfg(gatt_svr_svcs);
//   ble_gatts_add_svcs(gatt_svr_svcs);
  
  ESP_LOGI(TAG, "GATT services added, starting advertising");
  ble_hid_start_advertising();
}

static void host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");

    nimble_port_run();

    nimble_port_freertos_deinit();
}

static void nimble_task(void *param)
{
    ESP_LOGI(TAG, "NimBLE init");

    nimble_port_init();

    ble_svc_gap_init();
    ble_svc_gatt_init();

    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    ESP_LOGI(TAG, "count_cfg rc=%d", rc);
    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    ESP_LOGI(TAG, "add_svcs rc=%d", rc);

    ESP_LOGI(TAG, "report_char_handle = %d", report_chr_handle);

    ble_hs_cfg.reset_cb = NULL;
    ble_hs_cfg.sync_cb = ble_on_sync;

    nimble_port_freertos_init(host_task);

    while(1){
        vTaskDelay(portMAX_DELAY);
    }
}

void ble_hid_init(void) {
//   xTaskCreate(nimble_task, "nimble", 2048, NULL, 5, NULL);
  xTaskCreate(nimble_task, "nimble", 4096, NULL, 5, NULL);
}

// /* Optional: configure GPIO pins externally */
// esp_err_t ble_hid_set_pin_gpio(int gpio_num_btn1, int gpio_num_btn2) {
//   gpio_config_t io_conf = {
//     .pin_bit_mask = (1ULL<<gpio_num_btn1) | (1ULL<<gpio_num_btn2),
//     .mode = GPIO_MODE_INPUT,
//     .pull_up_en = GPIO_PULLUP_ENABLE,
//     .pull_down_en = GPIO_PULLDOWN_DISABLE,
//     .intr_type = GPIO_INTR_DISABLE
//   };
//   return gpio_config(&io_conf);
// }

bool disconnect_or_advertise(){
    ble_state_t state = ble_hid_get_state();
    if (state != BLE_STATE_CONNECTED){
        return false;
    }
    ble_store_clear();
    return true;
    
}

ble_state_t ble_hid_get_state(void)
{
    return ble_state;
}