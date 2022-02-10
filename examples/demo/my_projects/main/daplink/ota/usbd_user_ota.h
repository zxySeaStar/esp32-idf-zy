#ifndef USBD_USER_OTA_H_
#define USBD_USER_OTA_H_
#include <stddef.h>
#include <stdint.h>

// #define ID_OTA_BEGIN            0x00U
// #define ID_OTA_WRITE            0x01U
// #define ID_OTA_END              0x02U
// #define ID_OTA_SET_BOOT         0x03U
// #define ID_OTA_REBOOT           0x04U
enum ID_OTA_OPS
{
    ID_OTA_BEGIN = 1,
    ID_OTA_WRITE,
    ID_OTA_END,
    ID_OTA_SET_BOOT,
    ID_OTA_REBOOT,
};

extern uint32_t DAP_ProcessVendorCommand_OTA(const uint8_t *request, uint8_t *response);

#endif