#include <stdio.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include "usbd_user_ota.h"
#include "mbedtls/aes.h"
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

 unsigned char _firmware_aes_cbc_iv[16];
 unsigned char _firmware_aes_cbc_key[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0XD,0xE,0xF};

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
        // init iv key
        for(int i =0;i<16;i++)
        {
            _firmware_aes_cbc_iv[i]=0x1;
        }
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

    unsigned char dec_plain[64] = {0};
    mbedtls_aes_context aes_ctx;
    uint8_t len = *request;
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx,_firmware_aes_cbc_key,128);
    // handle data
    if(len%16==0)
    {
        mbedtls_aes_crypt_cbc(&aes_ctx,MBEDTLS_AES_DECRYPT,len,_firmware_aes_cbc_iv,(request+1),dec_plain);
    }
    else
    {
        *response = 0xFF;
        goto exit;
    }

    err = esp_ota_write(_ota_handle->update_handle, dec_plain, len);

    if(err!=ESP_OK)
    {
        *response = 0xFF;
        printf("[ERROR]\n");
    }
    else
    {
        *response = 1;
    }
exit:
    mbedtls_aes_free(&aes_ctx);
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