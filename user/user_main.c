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

#include "esp_common.h" 
#include "pwm.h"
#include "time.h"
#include "gpio.h"
#include "esp_alink.h"
#include "user_config.h"

#define SYSTEM_ON_OFF_COUNT     "reset_count"

os_timer_t timer;
uint8 type,x,y,z;
uint32 duty=0;
void ICACHE_FLASH_ATTR display(void *arg)
{
	for(y=0;y<3;y++)
	{
		for(z=0;z<12;z++)
		{
			duty=z*2000;
			type=y;
			pwm_set_duty(duty,type);
			pwm_start();
		}
	}
}
void user_pwm_init()
{
	uint32 pwm_duty_init[3]={0};
	uint32 io_info[][3]={{PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12,12},{PERIPHS_IO_MUX_MTCK_U,FUNC_GPIO13,13},{PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO15,15},};
	pwm_init(1000,pwm_duty_init,3,io_info);
	for(x=0;x<3;x++)
	{
		 type=x;
	     pwm_set_duty(duty,type);
		 pwm_start();
	}
	os_timer_disarm(&timer);
	os_timer_setfn(&timer,display,NULL);
	os_timer_arm(&timer,1000,1);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/ 
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

os_timer_t checkTimer_wifistate;
os_timer_t checkOnOffCnt;
os_timer_t smartConfigLed;

os_timer_t sntp_read_timer;
int timezone = 8;
static int smartConfigStatus = 0;

void ICACHE_FLASH_ATTR
sntp_read_timer_callback(void *arg)
{
	uint32_t time = sntp_get_current_timestamp();
	os_printf("sntp time:%d \n",time);
	os_printf("date:%s \n",sntp_get_real_time(time));
}
 
void ICACHE_FLASH_ATTR
my_sntp_init(void)
{
//	sntp_setservername(0,"0.cn.pool.ntp.org");
//	sntp_setservername(1,"1.cn.pool.ntp.org");
//	sntp_setservername(2,"2.cn.pool.ntp.org");
//	sntp_init();
 
	os_timer_disarm(&sntp_read_timer);
	os_timer_setfn(&sntp_read_timer, sntp_read_timer_callback , NULL);
	os_timer_arm(&sntp_read_timer,5000,1);
}

void Check_WifiState(void) {
    uint8 getState;
    getState = wifi_station_get_connect_status();

    if (getState == STATION_GOT_IP) {
        os_printf("wifi connect success \n");
        os_timer_disarm(&checkTimer_wifistate);
		os_printf("get time info \n");
		configTime(timezone, 0, "0.cn.pool.ntp.org", "1.cn.pool.ntp.org", "2.cn.pool.ntp.org", 1);
		my_sntp_init();
		led_off(5);
    }
	else {
		os_printf("wifi not connect \n");
		led_on(5);
	}

}

void ClearOnOffCnt(void) {
	os_printf("ClearOnOffCnt \n");
	esp_info_erase(SYSTEM_ON_OFF_COUNT);
}

void smartConfigControl(void) {
	uint8 getState;
    getState = wifi_station_get_connect_status();

    if (getState == STATION_GOT_IP) {
        os_printf("smartConfigControl wifi connect success\n");
        os_timer_disarm(&smartConfigLed);
		led_off(5);
    }
	else
	{
		led_toggle(5);
	}
}

static void smartConfigStart(void)
{
	struct station_config _wifi_config;
	
    if (esp_info_load("wifi_config", &_wifi_config, sizeof(struct station_config)) <= 0) {
		smartConfigStatus = 1;
	}
	
	os_timer_disarm(&smartConfigLed);
    os_timer_setfn(&smartConfigLed, (os_timer_func_t *) smartConfigControl, NULL);
    os_timer_arm(&smartConfigLed, 1000, true);

	user_demo_not_pass();
}

void ICACHE_FLASH_ATTR  user_init(void) 
{
/*
    start a demo project on esp8266 rtos sdk.
    run alink cloud connectting.
*/

	int ret = -1;
	int resetCnt = 0;
	led_init();
	os_printf(" uart init \n");
	user_uart_init();
	os_printf("key init  \n");
	alink_key_trigger();
	os_printf("esp init \n");
	esp_info_init();
	//os_printf("pwm init GPIO 12, 13, 15 \n");
#ifdef LED_OR_SWITCH
	os_printf("led_on 14");
	led_on(14);
#else
	led_off(14);
#endif

	ret = esp_info_load(SYSTEM_ON_OFF_COUNT, &resetCnt, sizeof(resetCnt));
	if (ret <= 0)
	{
		resetCnt = 1;
    	esp_info_save(SYSTEM_ON_OFF_COUNT, &resetCnt, sizeof(resetCnt));
	}
	else
	{
		resetCnt++;
		os_printf("reset_count = %d \n", resetCnt);
		if (resetCnt < 5)
    		esp_info_save(SYSTEM_ON_OFF_COUNT, &resetCnt, sizeof(resetCnt));
		else
		{
			ClearOnOffCnt();
			alink_event_send(ALINK_EVENT_UPDATE_ROUTER);
			alink_update_router();
		}
	}
	//check on/off count
	os_timer_disarm(&checkOnOffCnt);
    os_timer_setfn(&checkOnOffCnt, (os_timer_func_t *) ClearOnOffCnt, NULL);
    os_timer_arm(&checkOnOffCnt, 15000, false);

	smartConfigStart();

	//check wifi connect
	os_timer_disarm(&checkTimer_wifistate);
    os_timer_setfn(&checkTimer_wifistate, (os_timer_func_t *) Check_WifiState, NULL);
    os_timer_arm(&checkTimer_wifistate, 5000, true);
	
    return;
}
