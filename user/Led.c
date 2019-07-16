/******************************************************************************
 * Copyright 2013-2014 
 *
 * FileName:Led.c
 *
 * Description: 
 *
 * Modification history:
 *     2016/5/7, v1.0 create this file.
*******************************************************************************/

#include "esp_common.h"
#include "gpio.h"

/**
    * @brief  no .    
    * @note   no.
    * @param  no.
    * @retval no.
    */
void led_init( void )
{
	//os_printf("led_init 0 ,0 ,1\n");
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTMS_U);

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTDI_U);
	
	GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 0);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);
#ifdef LED_OR_SWITCH
	GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1);
#else
	GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0);
#endif
}
/**
    * @brief  no .    
    * @note   no.
    * @param  no.
    * @retval no.
    */
void led_on( uint8 gpio )
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio), 0);
	if (gpio == 14)
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1);
	}
}
/**
    * @brief  no .    
    * @note   no.
    * @param  no.
    * @retval no.
    */
void led_off( uint8 gpio )
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio), 1);
	if (gpio == 14)
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0);
	}
}


/**
    * @brief  no .    
    * @note   no.
    * @param  no.
    * @retval no.
    */
void led_toggle( uint8 gpio)
{
	uint32_t bit;
	
	bit = GPIO_INPUT_GET( GPIO_ID_PIN(gpio) );
	GPIO_OUTPUT_SET( GPIO_ID_PIN(gpio) , bit ^ 0x00000001 );
	if (gpio == 14)
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(12), bit);
	}
}

int getGpioStatus( uint8 gpio)
{
	return GPIO_INPUT_GET( GPIO_ID_PIN(gpio) );
}

