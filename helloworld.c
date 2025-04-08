/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"		// 參數集.
#include "xgpio.h"	// 簡化PS對PL的GPIO操作的函數庫.

// 定義初始延遲值
#define DEFAULT_DELAY 150
#define FAST_DELAY 40  // 按下按鈕後的較快閃爍速度

// 延遲用.
void delay(int dly)
{
    int i, j;
    for (i = 0; i < dly; i++) {
    	for (j = 0; j < 0xffff; j++) {
    		;
        }
    }
}

// 主程式.
int main()
{
    XGpio LED_XGpio, BTN_XGpio; // 宣告一個GPIO用的結構.
    int LED_num = 0b00000001;	// 初始值 LED0 開啟.
    int delay_time = DEFAULT_DELAY; // 初始閃爍速度
    int btn_state = 0;  // 按鈕狀態
    int direction = 1;  // 方向變數: 1 表示正向（從 0 到 7），-1 表示反向（從 7 到 0）

    XGpio_Initialize(&LED_XGpio, XPAR_AXI_GPIO_0_DEVICE_ID);	// 初始化LED_XGpio.
    XGpio_Initialize(&BTN_XGpio, XPAR_AXI_GPIO_1_DEVICE_ID);	// 初始化BTN_XGpio.

    XGpio_SetDataDirection(&LED_XGpio, 1, 0);	// LED 設為輸出
    XGpio_SetDataDirection(&BTN_XGpio, 1, 1);  // 按鈕設為輸入

    printf("Start!!!\n");

    while(1) {
        // 讀取按鈕狀態 (如果按下, `btn_state` 會變 1)
        btn_state = XGpio_DiscreteRead(&BTN_XGpio, 1);

        // 如果按鈕按下，減少延遲時間 (加快閃爍)
        if (btn_state) {
            delay_time = FAST_DELAY;
            printf("Button Pressed! Speeding up.\n");

        } else {
            delay_time = DEFAULT_DELAY;
        }

        // 顯示目前的LED狀態
        printf("LED_num = 0x%x\n", LED_num);

        // 將 LED 狀態輸出到 GPIO
        XGpio_DiscreteWrite(&LED_XGpio, 1, LED_num);	// LED_XGpio通道,送LED_num值進去.

        // 根據方向控制LED閃爍
        if (direction == 1) {
            // 正向 (從0到7)
            if (LED_num == 0b10000000) {  // 如果LED已經到達7，開始反向
                direction = -1;
            } else {
                LED_num <<= 1;  // LED_num 左移一位
            }
        } else {
            // 反向 (從7回到0)
            if (LED_num == 0b00000001) {  // 如果LED已經到達0，開始正向
                direction = 1;
            } else {
                LED_num >>= 1;  // LED_num 右移一位
            }
        }

        // 延遲
        delay(delay_time);
    }

    return 0;
}
