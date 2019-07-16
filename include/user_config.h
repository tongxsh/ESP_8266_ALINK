/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "esp_common.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "esp_alink.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define LED_OR_SWITCH    1   // define LED Code   ifndef SWITCH code
#define CONFIG_ALINK_MODULE_NAME    "ESP-WROOM-02"
#define USER_ALINK_GLOBAL_VER       "A[Alink1.0|56d2476]M[ESP8266]OS[1.3.0(68c9e7b]"

//=============== force sleep ==================
#define FIRST_START_FORCE_SLEEP  (1)
#define RECONNECT_FORCE_SLEEP    (1)
#define FPM_SLEEP_MAX_TIME       0xFFFFFFF
#define RECONNECT_SLEEP_TIME     (5*1000*1000)  // unit: us

#define ALINK_PASSTHROUGH_DATA_LEN  (8)

//============== alink device ==================
#ifdef LED_OR_SWITCH
#define DEV_NAME              "002xuanjikeji"
#define DEV_BRAND             "espressif"
#define DEV_CATEGORY          "LIVING"
#define DEV_TYPE              "LIGHT"
#define DEV_MANUFACTURE       "XINGTENGWENHUA"
#define DEV_MODEL             "XINGTENGWENHUA_LIVING_LIGHT_002XUANJIKEJI"
/* key/secret */
#define ALINK_KEY             "0D18GewMbqgFiWov9ZXF"
#define ALINK_SECRET          "vTMmb7fHxaU5telTY7Ponu3Y1hwxOirRHY0EnA6C"
/* sandbox key/secret */
#define ALINK_KEY_SANDBOX     "0D18GewMbqgFiWov9ZXF"
#define ALINK_SECRET_SANDBOX  "vTMmb7fHxaU5telTY7Ponu3Y1hwxOirRHY0EnA6C"
/* device hardware information */
#define DEV_SN                "041100020001"
#define DEV_VERSION           "1.0.2"
/* device MAC addr and chip ID, should acquired by call functions */
#define DEV_MAC               ""
#define DEV_CHIPID            ""   
#else
#define DEV_NAME              "001xuanji-test"
#define DEV_BRAND             "espressif"
#define DEV_CATEGORY          "LIVING"
#define DEV_TYPE              "OUTLET"
#define DEV_MANUFACTURE       "XINGTENGWENHUA"
#define DEV_MODEL             "XINGTENGWENHUA_LIVING_OUTLET_001XUANJI_TEST"
/* key/secret */
#define ALINK_KEY             "ydqK0AKZbuNj1AlJ4GWa"
#define ALINK_SECRET          "pn6IBToUciBvIvtHLl4IBY9WhMX4X6wsRB5p9caJ"
/* sandbox key/secret */
#define ALINK_KEY_SANDBOX     "ydqK0AKZbuNj1AlJ4GWa"
#define ALINK_SECRET_SANDBOX  "pn6IBToUciBvIvtHLl4IBY9WhMX4X6wsRB5p9caJ"
/* device hardware */
#define DEV_SN                "041100010001"
#define DEV_VERSION           "1.0.3"
/* device MAC addr and chip ID, should acquired by call functions */
#define DEV_MAC               ""             
#define DEV_CHIPID            ""   
#endif

#ifdef __cplusplus
}
#endif

#endif

