#include "sys.h"
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/


**************************************************************************/
//! 使用pid的的核心程序，详见control.c  -->  int TIM1_UP_IRQHandler(void) （定时器定时中断）

u8 Flag_Stop = 0, delay_50, delay_flag, Flash_Send;														 // 停止标志位 50ms精准延时标志位
int Encoder, Target_Position = 10000, Target_Velocity = 10;												 // 编码器的脉冲计数
int Moto;																								 // 电机PWM变量 应是Motor的 向Moto致敬
int Voltage;																							 // 电池电压采样相关的变量
float Position_KP = 120, Position_KI = 0.1, Position_KD = 500, Velocity_KP = 20, Velocity_KI = 30;		 // PID系数
float Amplitude_PKP = 20, Amplitude_PKI = 0.1, Amplitude_PKD = 25, Amplitude_VKP = 2, Amplitude_VKI = 3; // PID调试相关参数
float Menu_PID = 1, Menu_MODE = 1;
u16 PID_Parameter[10], Flash_Parameter[10]; // Flash相关数组
u8 Flag_OLED;
int main(void)
{
	Stm32_Clock_Init(9);		//=====系统时钟设置
	delay_init(72);				//=====延时初始化
	JTAG_Set(JTAG_SWD_DISABLE); //=====关闭JTAG接口
	JTAG_Set(SWD_ENABLE);		//=====打开SWD接口 可以利用主板的SWD接口调试
	delay_ms(1000);				//=====延时启动，等待系统稳定
	delay_ms(1000);				//=====延时启动，等待系统稳定 共2s
	LED_Init();					//=====初始化与 LED 连接的硬件接口
	EXTI_Init();				//=====按键初始化(外部中断的形式)
	OLED_Init();				//=====OLED初始化
	uart_init(72, 128000);		//=====初始化串口1

	MiniBalance_PWM_Init(7199, 0); //=====初始化PWM 10KHZ，用于驱动电机
	if (PAin(12) == 0)
		TIM2->CNT = 0;
	Encoder_Init_TIM4();   //=====初始化编码器（TIM2的编码器接口模式）
	Baterry_Adc_Init();	   //=====电池电压模拟量采集初始化
	Flash_Read();		   //=====从Flash里面读取参数
	Timer1_Init(99, 7199); //=====定时中断初始化
	while (1)
	{
		DataScope();	//===上位机
		delay_flag = 1; //===50ms中断精准延时标志位
		oled_show();	//===显示屏打开
		while (delay_flag)
			;				 //===50ms中断精准延时  主要是波形显示上位机需要严格的50ms传输周期
		if (Flash_Send == 1) // 写入PID参数到Flash,由按键控制该指令
		{
			Flash_Write();	// 把参数写入到Flash
			Flash_Send = 0; // 标志位清零
			Flag_OLED = 1;	// 显示标志位置1 在显示屏上面显示Data Is Saved的字样
		}
	}
}
