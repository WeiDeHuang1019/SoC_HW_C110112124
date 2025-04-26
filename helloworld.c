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

#include "xil_io.h"
#include "xparameters.h"

#define LEDrunning_BASEADDR XPAR_LEDRUNNING_0_S00_AXI_BASEADDR
#define normal_speed 5000000
#define fast_speed 2500000

void delay(int delay_time)
{
    volatile int i;
    for (i = 0; i < delay_time; i++);  // 簡單delay一下
}

int main()
{
    u32 led_data = 0x01; // 從最右邊第一顆開始
    int direction = 1;   // 1表示左移，-1表示右移
    int delay_time = 0;

    while(1)
    {
        Xil_Out32(LEDrunning_BASEADDR, led_data); // 寫資料到IP控制LED
        u32 show = Xil_In32(LEDrunning_BASEADDR+0x04);

        printf("%lu\r\n",show);
        if(show == 1)
        	delay_time = fast_speed;
        else
        	delay_time = normal_speed;
        printf("%d\r\n",delay_time);

        delay(delay_time); // 看得出跑馬燈效果

        if (direction == 1)
        {
            led_data <<= 1; // 向左移動
            if (led_data & 0x80) // 最左邊的LED亮了
            {
                direction = -1; // 變成向右跑
            }
        }
        else // direction == -1
        {
            led_data >>= 1; // 向右移動
            if (led_data & 0x01) // 最右邊的LED亮了
            {
                direction = 1; // 變成向左跑
            }
        }
    }

    return 0;
}
