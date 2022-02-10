#include <stdint.h>

#define CONFIG_TINYUSB_HID_ENABLED 1
#include "esp_log.h"
#include "tinyusb.h"

#include "usbd_user_hid.h"
#include "info.h"


static const char *TAG = "example";
void app_main(void)
{
    DAP_Setup();

    info_init();

    usbd_hid_init();

    ESP_LOGI(TAG, "USB initialization");
    tinyusb_config_t tusb_cfg = {}; // the configuration using default values
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
}


//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
ESP_LOGI(TAG, "%s,Got data (%d bytes): %s", __FUNCTION__,reqlen, buffer);
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // This example doesn't use multiple report and report ID
  (void) itf;
  (void) report_id;
  (void) report_type;

  // echo back anything we received from host
  //ESP_LOGI(TAG, "%s,Got data (%d bytes): %s", __FUNCTION__, bufsize, buffer);
  //printf("rtype:%d\n",report_type);
  //  printf("received:");
  //  for(int i =0;i<32;i++)
  //  {
  //    printf("%d ",buffer[i]);
  //  }
  //  printf("\n");
  //tud_hid_report(0, buffer, bufsize);
  usbd_hid_set_report(report_type,report_id,buffer,bufsize,USBD_HID_REQ_EP_CTRL);
}


// void tud_hid_report_complete_cb(uint8_t itf, uint8_t const* report, uint8_t len)
// {
// (void) itf;
//   (void) len;

//   uint8_t next_report_id = report[0] + 1;

//   hid_send_packet(next_report_id);

// }