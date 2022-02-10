/**
 * @file    info.c
 * @brief   Implementation of info.h
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
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

#include "info.h"
#include "string.h"

static char string_product[20 + 1];
static char string_unique_id[40 + 1];


void info_init(void)
{
    const char* str = "CMSIS-DAP";
    const char* str2 = "0700000036543438ff015050ffff2fac";

    memset(string_product,0,sizeof(string_product));
    memcpy(string_product,str, strlen(str));

    memset(string_unique_id,0,sizeof(string_unique_id));
    memcpy(string_unique_id,str2, strlen(str2));

}

const char* info_get_product(void)
{
    return string_product;
}


const char* info_get_unique_id(void)
{
    return string_unique_id;
}


