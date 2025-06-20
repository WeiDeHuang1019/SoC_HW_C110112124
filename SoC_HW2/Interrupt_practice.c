#include <stdio.h>
#include "platform.h"

#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xtmrctr.h"

#define Z700_INTC_ID      XPAR_PS7_SCUGIC_0_DEVICE_ID
#define BTN_ID            XPAR_AXI_GPIO_1_DEVICE_ID
#define LED_ID            XPAR_AXI_GPIO_0_DEVICE_ID
#define TIMER_ID          XPAR_AXI_TIMER_0_DEVICE_ID

#define INTC_GPIO_ID      XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR
#define INTC_TIMER_ID     XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR

#define BTN_INT           XGPIO_IR_CH1_MASK
#define GIC_ID            XPAR_SCUGIC_SINGLE_DEVICE_ID

XGpio LED, BTN;
XScuGic INTCInst;
XTmrCtr TimerInst;

int LED_num = 0;
int Intr_CTN = 0;

// ================================
// 按鈕中斷處理函式
void Intr_Handler(void *CallbackRef) {
    XGpio *GpioPtr = (XGpio *)CallbackRef;

    XGpio_InterruptDisable(GpioPtr, BTN_INT);          // ✅ 先 disable
    XGpio_InterruptClear(GpioPtr, 0xFFFFFFFF);          // ✅ 清除所有 flags（更保險）

    Intr_CTN++;
    printf("Interrupt : %d\n", Intr_CTN);

    LED_num = 0;

    XGpio_InterruptEnable(GpioPtr, BTN_INT);            // ✅ 最後再打開
}




// Timer 中斷處理函式
volatile int timer_flag = 0;

void Timer_Intr_Handler(void *CallBackRef, u8 TmrCtrNumber) {
    if (XTmrCtr_IsExpired(&TimerInst, TmrCtrNumber)) {
        XTmrCtr_Reset(&TimerInst, TmrCtrNumber);
        LED_num++;
        timer_flag = 1;
    }
}


// ================================
// 中斷初始化
void Intr_Setup(XScuGic *GicInstancePtr, XGpio *GpioInstancePtr) {
    XScuGic_Config *IntcConfig;
    IntcConfig = XScuGic_LookupConfig(GIC_ID);
    XScuGic_CfgInitialize(GicInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
        (Xil_ExceptionHandler)XScuGic_InterruptHandler, GicInstancePtr);
    Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

    // GPIO 中斷
    XScuGic_Connect(GicInstancePtr, INTC_GPIO_ID, (Xil_ExceptionHandler)Intr_Handler, (void *)GpioInstancePtr);
    XScuGic_Enable(GicInstancePtr, INTC_GPIO_ID);
    XGpio_InterruptGlobalEnable(GpioInstancePtr);
    XGpio_InterruptEnable(GpioInstancePtr, BTN_INT);

    // Timer 中斷
    XScuGic_Connect(GicInstancePtr, INTC_TIMER_ID, (Xil_ExceptionHandler)Timer_Intr_Handler, (void *)&TimerInst);
    XScuGic_Enable(GicInstancePtr, INTC_TIMER_ID);
}

// Timer 初始化
void Timer_Setup() {
    XTmrCtr_Initialize(&TimerInst, TIMER_ID);
    XTmrCtr_SetOptions(&TimerInst, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
    XTmrCtr_SetResetValue(&TimerInst, 0, 0x80000000);  // 約每秒中斷一次（視時脈調整）
    XTmrCtr_Start(&TimerInst, 0);
}

// 延遲函式
void delay(int ms) {
    for (int i = 0; i < ms * 10000; i++) {
        asm("nop");
    }
}

// ================================
int main() {
    init_platform();

    XGpio_Initialize(&LED, LED_ID);
    XGpio_SetDataDirection(&LED, 1, 0);  // CH1 為輸出

    XGpio_Initialize(&BTN, BTN_ID);
    XGpio_SetDataDirection(&BTN, 1, 1);  // CH1 為輸入

    Intr_Setup(&INTCInst, &BTN);  // 初始化中斷系統
    Timer_Setup();                // 啟動 Timer 中斷

    print("Init successful\n");

    while (1) {
        XGpio_DiscreteWrite(&LED, 1, LED_num);



        delay(500);
    }
}
