
#ifndef USBD_USER_HID_H_
#define USBD_USER_HID_H_
#include "DAP_queue.h"
#include "tinyusb.h"
/* USB HID Class API enumerated constants                                     */
enum {
    USBD_HID_REQ_EP_CTRL = 0,             /* Request from control endpoint      */
    USBD_HID_REQ_EP_INT,                  /* Request from interrupt endpoint    */
    USBD_HID_REQ_PERIOD_UPDATE            /* Request from periodic update       */
};
extern void  usbd_hid_init(void);
extern void  usbd_hid_set_report(U8 rtype, U8 rid, U8 *buf, int len, U8 req);
extern void  hid_send_packet(U8 rid);

#define ID_OTA_CMD                     0xF0U


#endif