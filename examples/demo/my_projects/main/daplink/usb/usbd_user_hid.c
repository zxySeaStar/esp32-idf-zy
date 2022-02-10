/**
 * @file    usbd_user_hid.c
 * @brief   HID driver for CMSIS-DAP packet processing
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include "DAP_config.h"  // daplink芯片相关的读写操作
#include "DAP.h"

#include "DAP_queue.h"

#define USBD_HID_OUTREPORT_MAX_SZ   CFG_TUD_HID_BUFSIZE


#include "usbd_user_hid.h"
#include "usbd_user_ota.h"

/* HID Report Types */
#define HID_REPORT_INPUT                0x01
#define HID_REPORT_OUTPUT               0x02
#define HID_REPORT_FEATURE              0x03

#if (USBD_HID_OUTREPORT_MAX_SZ > DAP_PACKET_SIZE)
#error "USB HID Output Report Size must be less than DAP Packet Size"
#endif
#if (USBD_HID_INREPORT_MAX_SZ > DAP_PACKET_SIZE)
#error "USB HID Input Report Size must be less than DAP Packet Size"
#endif

static volatile uint8_t  USB_ResponseIdle;
static DAP_queue DAP_Cmd_queue;

void hid_send_packet(U8 rid)
{
    uint8_t * sbuf;
    int slen;
    if (DAP_queue_get_send_buf(&DAP_Cmd_queue, &sbuf, &slen)) {
        if (slen > USBD_HID_OUTREPORT_MAX_SZ){
            //util_assert(0);
            
        }else {
            //usbd_hid_get_report_trigger(0, sbuf, USBD_HID_OUTREPORT_MAX_SZ);
            //  printf("send:");
            // for(int i =0;i<slen;i++)
            // {
            //     printf("%d ",sbuf[i]);
            // }
            // printf("\n");
            
            // int tempcount = 0;
            // int temp=20000;
            // while(tempcount<3)
            // {
            //     uint8_t tempresult = tud_hid_n_ready(0);
            //     printf("%d:%d\n",tempcount,tempresult);
            //     if(tempresult==0)
            //     {
            //         temp=20000;
            //         while(temp-->0);
            //         tempcount++;
            //     }
            //     else{
            //         break;
            //     }
            // }
            
            bool result = tud_hid_report_16(rid, sbuf, USBD_HID_OUTREPORT_MAX_SZ);
            int temp=20000;
            while(temp-->0);

            if(!result)
                printf("*************************************************\n");
        }
    }
}

// USB HID Callback: when system initializes
void usbd_hid_init(void)
{
    USB_ResponseIdle = 1;
    DAP_queue_init(&DAP_Cmd_queue);
}

// USB HID Callback: when data needs to be prepared for the host
int usbd_hid_get_report(U8 rtype, U8 rid, U8 *buf, U8 req)
{
    uint8_t * sbuf;
    int slen;
    switch (rtype) {
        case HID_REPORT_INPUT:
            switch (req) {
                case USBD_HID_REQ_PERIOD_UPDATE:
                    break;

                case USBD_HID_REQ_EP_CTRL:
                case USBD_HID_REQ_EP_INT:
                    if (DAP_queue_get_send_buf(&DAP_Cmd_queue, &sbuf, &slen)) {
                        if (slen > USBD_HID_OUTREPORT_MAX_SZ){
                            //util_assert(0);
                        }else {
                            memcpy(buf, sbuf, slen);
                            return (USBD_HID_OUTREPORT_MAX_SZ);
                        }
                    } else if (req == USBD_HID_REQ_EP_INT) {
                        USB_ResponseIdle = 1;
                    }
                    break;
            }

            break;

        case HID_REPORT_FEATURE:
            break;
    }

    return (0);
}

// USB HID override function return 1 if the activity is trivial or response is null
__attribute__((weak))
uint8_t usbd_hid_no_activity(U8 *buf)
{
    return 0;
}


// USB HID Callback: when data is received from the host
void usbd_hid_set_report(U8 rtype, U8 rid, U8 *buf, int len, U8 req)
{
    uint8_t    USB_Request [DAP_PACKET_SIZE+1];
    uint8_t * rbuf;
    //main_led_state_t led_next_state = MAIN_LED_FLASH;
    switch (rtype) {
        case HID_REPORT_OUTPUT:
        case 0:
            if (len == 0) {
                break;
            }

            if (buf[0] == ID_DAP_TransferAbort) {
                DAP_TransferAbort = 1;
                break;
            }

            // execute and store to DAP_queue
            if (DAP_queue_execute_buf(&DAP_Cmd_queue, buf, len, &rbuf)) {
                if(usbd_hid_no_activity(rbuf) == 1){
                    //revert HID LED to default if the response is null
                    //led_next_state = MAIN_LED_DEF;
                }
                if (USB_ResponseIdle || true) {
                    hid_send_packet(rid);
                    USB_ResponseIdle = 0;
                }
            } else {
                //util_assert(0);
                printf("free count 0\n");
            }

            //main_blink_hid_led(led_next_state);
            //memcpy(USB_Request, buf, len);
            break;

        case HID_REPORT_FEATURE:
            break;
    }
}

// Process DAP Vendor extended command request and prepare response
// Default function (can be overridden)
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
uint32_t DAP_ProcessVendorCommandEx(const uint8_t *request, uint8_t *response) {
    uint32_t num;
    // from 0xA0 to 0xFF
    *response++ = *request;
    switch(*request++)
    {
        case ID_OTA_CMD:
            // ota
            num = DAP_ProcessVendorCommand_OTA(request,response);
            break;
        default:
            *(response-1) = ID_DAP_Invalid;
            return ((1U << 16) | 1U);
    }
    return ((1U << 16) | (1U + num));
}


