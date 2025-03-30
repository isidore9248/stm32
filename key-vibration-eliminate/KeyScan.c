#include "STM32Headfile.h"
#include <key.h>

int main(void)
{
	struct KEY_Monitoring_Data Key1;
	KEY_Init(&Key1, GPIO_Pin_4, GPIOA, RCC_APB2Periph_GPIOA);

	static int BEEP = 0, LED = 0;

	while (1)
	{
		if (KEY_Can_Scan())
		{
			// 50HZ
			KEY_Scan(&Key1);
			// 单击
			if (Key1.state == CLICKED)
			{
				LED = 0;
			}
			// 双击
			else if (Key1.state == DOUBLE_CLICKED)
			{
				LED = 1;
				BEEP = 0;
			}
			// 长按
			else if (Key1.state == LONG_CLICKED)
			{
				BEEP = 1;
			}
			else
			{
			}
			KEY_CleanState(&Key1);
			KEY_CleanScanFlag();
		}
	}
}

void TIM4_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	KEY_Scan_TIMProcess();
}