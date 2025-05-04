#include "exti.h"
#include "key.h"
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
/**************************************************************************
函数功能：外部中断初始化
入口参数：无
返回  值：无
**************************************************************************/
void EXTI_Init(void)
{
	KEY_Init();
	if (PAin(12) == 0)
	{
		Menu_MODE = 2; // 按住A12按键启动，进入速度模式
		TIM4->CNT = 0;
	}
	Ex_NVIC_Config(GPIO_A, 12, FTIR); // 下降沿触发
	Ex_NVIC_Config(GPIO_A, 7, FTIR);  // 下降沿触发
	Ex_NVIC_Config(GPIO_A, 5, FTIR);  // 下降沿触发
	Ex_NVIC_Config(GPIO_A, 11, FTIR); // 下降沿触发

	MY_NVIC_Init(2, 2, EXTI9_5_IRQn, 2);   // 抢占2，子优先级2，组2
	MY_NVIC_Init(2, 2, EXTI15_10_IRQn, 2); // 抢占2，子优先级2，组2
}

// 外部中断9~5服务程序
void EXTI9_5_IRQHandler(void)
{
	delay_ms(4);   // 消抖
	if (KEY5 == 0) //
	{
		Flag_Stop = !Flag_Stop;
	}
	if (KEY7 == 0) /// Menu
	{
		if (Menu_MODE == 1)
		{
			if (++Menu_PID > 3)
				Menu_PID = 1;
		}
		else
		{
			if (++Menu_PID > 2)
				Menu_PID = 1;
		}
	}
	EXTI->PR = 1 << 5; // 清除LINE5上的中断标志位
	EXTI->PR = 1 << 7; // 清除LINE7上的中断标志位
}
// 外部中断15~10服务程序
void EXTI15_10_IRQHandler(void)
{
	delay_ms(4); // 消抖
	if (Menu_MODE == 1)
	{
		if (KEY11 == 0) // PID-
		{
			if (Menu_PID == 1)
				Position_KP -= Amplitude_PKP;
			else if (Menu_PID == 2)
				Position_KI -= Amplitude_PKI;
			else if (Menu_PID == 3)
				Position_KD -= Amplitude_PKD;
		}
		if (KEY12 == 0) // PID+
		{
			if (Menu_PID == 1)
				Position_KP += Amplitude_PKP;
			else if (Menu_PID == 2)
				Position_KI += Amplitude_PKI;
			else if (Menu_PID == 3)
				Position_KD += Amplitude_PKD;
		}
	}
	else
	{
		if (KEY11 == 0) // PID-
		{
			if (Menu_PID == 1)
				Velocity_KP -= Amplitude_VKP;
			else if (Menu_PID == 2)
				Velocity_KI -= Amplitude_VKI;
		}
		if (KEY12 == 0) // PID+
		{
			if (Menu_PID == 1)
				Velocity_KP += Amplitude_VKP;
			else if (Menu_PID == 2)
				Velocity_KI += Amplitude_VKI;
		}
	}
	if (Velocity_KP <= 0)
		Velocity_KP = 0;
	if (Velocity_KI <= 0)
		Velocity_KI = 0;
	if (Position_KD <= 0)
		Position_KD = 0;
	if (Position_KP <= 0)
		Position_KP = 0;
	if (Position_KI <= 0)
		Position_KI = 0;
	EXTI->PR = 1 << 11; // 清除LINE11上的中断标志位
	EXTI->PR = 1 << 12; // 清除LINE12上的中断标志位
}
