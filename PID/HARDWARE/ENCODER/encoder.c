#include "encoder.h"
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
/**************************************************************************
函数功能：把TIM2初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM2(void)
{
	RCC->APB1ENR |= 1 << 0;	  // TIM2时钟使能
	RCC->APB2ENR |= 1 << 2;	  // 使能PORTA时钟
	GPIOA->CRL &= 0XFFFFFF00; // PA0 PA1
	GPIOA->CRL |= 0X00000044; // 浮空输入
	/* 把定时器初始化为编码器模式 */
	TIM2->DIER |= 1 << 0; // 允许更新中断
	TIM2->DIER |= 1 << 6; // 允许触发中断
	MY_NVIC_Init(1, 3, TIM2_IRQn, 1);

	/* Timer configuration in Encoder mode */
	TIM2->PSC = 0x0;				// 预分频器
	TIM2->ARR = ENCODER_TIM_PERIOD; // 设定计数器自动重装值
	TIM2->CR1 &= ~(3 << 8);			// 选择时钟分频：不分频
	TIM2->CR1 &= ~(3 << 5);			// 选择计数模式:边沿对齐模式

	TIM2->CCMR1 |= 1 << 0;	 // CC1S='01' IC1FP1映射到TI1
	TIM2->CCMR1 |= 1 << 8;	 // CC2S='01' IC2FP2映射到TI2
	TIM2->CCER &= ~(1 << 1); // CC1P='0'	 IC1FP1不反相，IC1FP1=TI1
	TIM2->CCER &= ~(1 << 5); // CC2P='0'	 IC2FP2不反相，IC2FP2=TI2
	TIM2->CCMR1 |= 3 << 4;	 //	IC1F='1000' 输入捕获1滤波器
	TIM2->SMCR |= 3 << 0;	 // SMS='011' 所有的输入均在上升沿和下降沿有效
	if (Menu_MODE == 1)
		TIM2->CNT = 10000;
	else
		TIM2->CNT = 0;
	TIM2->CR1 |= 0x01; // CEN=1，使能定时器
}
/**************************************************************************
函数功能：把TIM4初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM4(void)
{
	RCC->APB1ENR |= 1 << 2;	  // TIM4时钟使能
	RCC->APB2ENR |= 1 << 3;	  // 使能PORTB时钟
	GPIOB->CRL &= 0X00FFFFFF; // PB6 PB7
	GPIOB->CRL |= 0X44000000; // 浮空输入
	/* Enable the TIM3 Update Interrupt */
	// 这两个东东要同时设置才可以使用中断
	TIM4->DIER |= 1 << 0; // 允许更新中断
	TIM4->DIER |= 1 << 6; // 允许触发中断
	MY_NVIC_Init(1, 3, TIM4_IRQn, 1);

	/* Timer configuration in Encoder mode */
	TIM4->PSC = 0x0;				// 预分频器
	TIM4->ARR = ENCODER_TIM_PERIOD; // 设定计数器自动重装值
	TIM4->CR1 &= ~(3 << 8);			// 选择时钟分频：不分频
	TIM4->CR1 &= ~(3 << 5);			// 选择计数模式:边沿对齐模式

	TIM4->CCMR1 |= 1 << 0;	 // CC1S='01' IC1FP1映射到TI1
	TIM4->CCMR1 |= 1 << 8;	 // CC2S='01' IC2FP2映射到TI2
	TIM4->CCER &= ~(1 << 1); // CC1P='0'	 IC1FP1不反相，IC1FP1=TI1
	TIM4->CCER &= ~(1 << 5); // CC2P='0'	 IC2FP2不反相，IC2FP2=TI2
	TIM4->CCMR1 |= 3 << 4;	 //	IC1F='1000' 输入捕获1滤波器
	TIM4->SMCR |= 3 << 0;	 // SMS='011' 所有的输入均在上升沿和下降沿有效
	if (Menu_MODE == 1)
		TIM4->CNT = 10000;
	else
		TIM4->CNT = 0;
	TIM4->CR1 |= 0x01; // CEN=1，使能定时器
}
/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int Read_Velocity(u8 TIMX)
{
	int Encoder_TIM;
	switch (TIMX)
	{
	case 2:
		Encoder_TIM = (short)TIM2->CNT;
		TIM2->CNT = 0;
		break;
	case 3:
		Encoder_TIM = (short)TIM3->CNT;
		TIM3->CNT = 0;
		break;
	case 4:
		Encoder_TIM = (short)TIM4->CNT;
		TIM4->CNT = 0;
		break;
	default:
		Encoder_TIM = 0;
	}
	return Encoder_TIM;
}
/**************************************************************************
函数功能：读取位置信息
入口参数：定时器
返回  值：位置值
**************************************************************************/
int Read_Position(u8 TIMX)
{
	int Encoder_TIM;
	switch (TIMX)
	{
	case 2:
		Encoder_TIM = (short)TIM2->CNT;
		break;
	case 3:
		Encoder_TIM = (short)TIM3->CNT;
		break;
	case 4:
		Encoder_TIM = (short)TIM4->CNT;
		break;
	default:
		Encoder_TIM = 0;
	}
	return Encoder_TIM;
}
void TIM4_IRQHandler(void)
{
	if (TIM4->SR & 0X0001) // 溢出中断
	{
	}
	TIM4->SR &= ~(1 << 0); // 清除中断标志位
}
void TIM2_IRQHandler(void)
{
	if (TIM2->SR & 0X0001) // 溢出中断
	{
	}
	TIM2->SR &= ~(1 << 0); // 清除中断标志位
}
