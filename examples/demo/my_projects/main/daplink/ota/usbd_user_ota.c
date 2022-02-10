#include <stdio.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include "usbd_user_ota.h"

/*
 *  Function Declare 
 */
void OtaCommandBegin();
uint32_t OtaCommandWrite(const uint8_t *request, uint8_t *response);
void OtaCommandEnd();
void OtaCommandSetBoot();
void OtaCommandReboot();

/*
 * Struct Declare
 */

struct usbd_user_ota_handle {
    esp_ota_handle_t update_handle;
    const esp_partition_t *update_partition;
    // esp_http_client_handle_t http_client;
    char *ota_upgrade_buf;
    // size_t ota_upgrade_buf_size;
    // int binary_file_len;
    // int image_length;
    // int max_http_request_size;
    // esp_https_ota_state state;
    // bool bulk_flash_erase;
    // bool partial_http_download;
};
typedef struct usbd_user_ota_handle usbd_user_ota_t;


/*
 * Member Declare
 */
static usbd_user_ota_t* _ota_handle = NULL;
static bool             _ota_inited = false;

uint32_t DAP_ProcessVendorCommand_OTA(const uint8_t *request, uint8_t *response)
{
    uint32_t num = 0;
    //printf("%s,%d\n",__FUNCTION__,__LINE__);
    *response++ = *request;
    switch(*request++)
    {
        case ID_OTA_BEGIN:
            OtaCommandBegin();
            break;
        case ID_OTA_WRITE:
            num = OtaCommandWrite(request,response);
            break;
        case ID_OTA_END:
            OtaCommandEnd();
            break;
        case ID_OTA_SET_BOOT:
            OtaCommandSetBoot();
            break;
        case ID_OTA_REBOOT:
            OtaCommandReboot();
            break;
        default:
            num = 20;
           break;
    }
    return ( 1U << 16) | (1U + num);
}

void OtaCommandBegin()
{
    esp_err_t err;
    _ota_inited = false;
    // init handle
    if(_ota_handle == NULL)
    {
        _ota_handle = calloc(1, sizeof(usbd_user_ota_t));
    }

    if(_ota_handle == NULL)
    {
        printf("failed to assign buffer for ota_handle\n");
        return;
    }
    else
    {
        if(_ota_handle->update_handle!=0)
        {
            printf("abort history handle\n");
            err = esp_ota_abort(_ota_handle->update_handle);
            if(err!=ESP_OK)
            {
                printf("failed to abort history handle\n");
                return;
            }
        }
    }
        
    _ota_handle->update_partition = esp_ota_get_next_update_partition(NULL);
    if(_ota_handle->update_partition)
    {
        const esp_partition_t * temp = _ota_handle->update_partition;
        printf("success\n");
        printf("%d %d %d %s \n", temp->size, temp->address,temp->type,temp->label);
    }
    else
    {
        printf("failed\n");
        return;
    }

    err = esp_ota_begin(_ota_handle->update_partition, OTA_WITH_SEQUENTIAL_WRITES,&_ota_handle->update_handle);
    if(err!=ESP_OK)
    {
        printf("failed to call esp_ota_begin()\n");
    }
    else
    {
        _ota_inited = true;
    }
}

uint32_t OtaCommandWrite(const uint8_t *request, uint8_t *response)
{
     esp_err_t err;
    if(!_ota_inited)
    {
        printf("ota not inited\n");
        return (1U << 16) + 1U;;
    }

    err = esp_ota_write(_ota_handle->update_handle, (request+1), *request);
    if(err!=ESP_OK)
    {
        *response = 0xFF;
    }
    else
    {
        *response = 1;
    }
    
    return (1U << 16) + 2U;
}

void OtaCommandEnd()
{
    esp_err_t err;
    if(!_ota_inited)
    {
        printf("ota not inited\n");
        return;
    }

    err = esp_ota_end(_ota_handle->update_handle);
    if(err!=ESP_OK)
    {
        printf("failed to call esp_ota_end(),error code:%d\n",err);
    }
    else
    {
        err = esp_ota_set_boot_partition(_ota_handle->update_partition);
    }
}

void OtaCommandSetBoot()
{

}

void OtaCommandReboot()
{
    esp_restart();
}