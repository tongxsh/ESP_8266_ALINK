/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "uart.h"

enum {
    UART_EVENT_RX_CHAR,
    UART_EVENT_MAX
};

typedef struct _os_event_ {
    uint32 event;
    uint8  *param;
} os_event_t;

xTaskHandle xUartTaskHandle;
xQueueHandle xQueueUart;

LOCAL STATUS
uart_tx_one_char(uint8 uart, uint8 TxChar)
{
    while (true) {
        uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);

        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
            break;
        }
    }

    WRITE_PERI_REG(UART_FIFO(uart) , TxChar);
    return OK;
}

LOCAL void
uart1_write_char(char c)
{
    if (c == '\n') {
        uart_tx_one_char(UART1, '\r');
        uart_tx_one_char(UART1, '\n');
    } else if (c == '\r') {
    } else {
        uart_tx_one_char(UART1, c);
    }
}

LOCAL void
uart0_write_char(char c)
{
    if (c == '\n') {
        uart_tx_one_char(UART0, '\r');
        uart_tx_one_char(UART0, '\n');
    } else if (c == '\r') {
    } else {
        uart_tx_one_char(UART0, c);
    }
}

void uart0_SendStr(char* Str)
{
	while(*Str)
	{
		uart0_write_char(*Str++);
	}
}

LOCAL void
user_uart_task(void *pvParameters)
{
    os_event_t e;
    for (;;) {
        if (xQueueReceive(xQueueUart, (void *)&e, (portTickType)portMAX_DELAY)) {
            switch (e.event) {
                case UART_EVENT_RX_CHAR:
					uart0_SendStr(e.param);  
					uart_data_parse(e.param);
                    break;

                default:
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}

LOCAL void
user_uart0_rx_intr_handler(void *para)
{
    os_event_t e;
    uint8 RcvChar;
    uint8 uart_no = UART0;//UartDev.buff_uart_no;
    uint8 fifo_len = 0;
    uint8 buf_idx = 0;
    uint8 fifo_tmp[128] = {0};

    portBASE_TYPE xHigherPriorityTaskWoken;

    uint32 uart_intr_status = READ_PERI_REG(UART_INT_ST(uart_no)) ;

    while (uart_intr_status != 0x0) {
         if (UART_FRM_ERR_INT_ST == (uart_intr_status & UART_FRM_ERR_INT_ST)) {
            //printf("FRM_ERR\r\n");
            WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_FRM_ERR_INT_CLR);
        } else if (UART_RXFIFO_FULL_INT_ST == (uart_intr_status & UART_RXFIFO_FULL_INT_ST)) {
            printf("full\r\n");
            fifo_len = (READ_PERI_REG(UART_STATUS(UART0)) >> UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
            buf_idx = 0;
/*
            while (buf_idx < fifo_len) {
                uart_tx_one_char(UART0, READ_PERI_REG(UART_FIFO(UART0)) & 0xFF);
                buf_idx++;
            }
*/
            WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
        } else if (UART_RXFIFO_TOUT_INT_ST == (uart_intr_status & UART_RXFIFO_TOUT_INT_ST)) {
            fifo_len = (READ_PERI_REG(UART_STATUS(UART0)) >> UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
            buf_idx = 0;

            /*while (buf_idx < fifo_len) {
                uart_tx_one_char(UART0, READ_PERI_REG(UART_FIFO(UART0)) & 0xFF);
                buf_idx++;
            }*/

            while (buf_idx < fifo_len) {
                fifo_tmp[buf_idx] = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF; // 把串口fifo里面的数据循环取出来并放到字符数组里面去
                buf_idx++;
            }
            fifo_tmp[buf_idx] = '\0';
            WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);

            e.event = UART_EVENT_RX_CHAR; // 设置好事件，便于任务查看
            e.param = fifo_tmp;            // 把串口接收到的数据放到结构体中去

            // 使用带串口中断保护的消息队列发送函数，这个函数只能用于中断服务函数中，其他地方绝对不能使用！！
            xQueueSendFromISR(xQueueUart, (void *)&e, &xHigherPriorityTaskWoken);
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

        } else if (UART_TXFIFO_EMPTY_INT_ST == (uart_intr_status & UART_TXFIFO_EMPTY_INT_ST)) {
            printf("empty\n\r");
            WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_TXFIFO_EMPTY_INT_CLR);
            CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
        } else {
            //skip
        }

        uart_intr_status = READ_PERI_REG(UART_INT_ST(uart_no)) ;
    }
}

void
user_uart_init(void)
{
    UART_WaitTxFifoEmpty(UART0);
    UART_WaitTxFifoEmpty(UART1);

    UART_ConfigTypeDef uart_config;
    uart_config.baud_rate    = BIT_RATE_115200;
    uart_config.data_bits    = UART_WordLength_8b;
    uart_config.parity       = USART_Parity_None;
    uart_config.stop_bits    = USART_StopBits_1;
    uart_config.flow_ctrl    = USART_HardwareFlowControl_None;
    uart_config.UART_RxFlowThresh = 120;
    uart_config.UART_InverseMask = UART_None_Inverse;
    UART_ParamConfig(UART0, &uart_config);

    UART_IntrConfTypeDef uart_intr;
    uart_intr.UART_IntrEnMask = UART_RXFIFO_TOUT_INT_ENA | UART_FRM_ERR_INT_ENA | UART_RXFIFO_FULL_INT_ENA | UART_TXFIFO_EMPTY_INT_ENA;
    uart_intr.UART_RX_FifoFullIntrThresh = 100; // 设置100个字符就溢出
    uart_intr.UART_RX_TimeOutIntrThresh = 10;
    uart_intr.UART_TX_FifoEmptyIntrThresh = 20;
    UART_IntrConfig(UART0, &uart_intr);

    UART_SetPrintPort(UART0);
    UART_intr_handler_register(user_uart0_rx_intr_handler, NULL); // 注册串口中断函数处理函数
    ETS_UART_INTR_ENABLE();
    // 中断设置完成以后就马上初始化队列和创建串口处理任务

    xQueueUart = xQueueCreate(32, sizeof(os_event_t));

   xTaskCreate(user_uart_task, (uint8 const *)"uTask", 512, NULL, 5, &xUartTaskHandle);
}




