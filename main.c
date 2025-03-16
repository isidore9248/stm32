#include "stm32f10x.h"

u8 key_scan_flag = 0;
extern struct KEYdataBase KeyData;



int main(void)
{
    KEY_Init();
    TIM4_KeyScan_Init(1000 - 1, 72 - 1); // 72MHZ/72=1MHZ 1ms时基

    while (1)
    {
        if (key_scan_flag)
        { // 50HZ
            KEY_Scan();
            if (KeyData.KEY1 == CLICKED) // 单击
            {
                LED = 0;
                KeyData.KEY1 = NO_CLICKED;
            }
            else if (KeyData.KEY1 == DOUBLE_CLICKED) // 双击
            {
                LED = 1;
                BEEP = 0;
                KeyData.KEY1 = NO_CLICKED;
            }
            else if (KeyData.KEY1 == LONG_CLICKED)
            { // 长按
                BEEP = 1;
                KeyData.KEY1 = NO_CLICKED;
            }
            else
            {
            }
            key_scan_flag = 0;
        }
    }
}

void TIM4_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    static u8 counter1 = 0;

    counter1++;
    if (counter1 == 20) // 20ms
    {
        counter1 = 0;
        key_scan_flag = 1;
    }
}