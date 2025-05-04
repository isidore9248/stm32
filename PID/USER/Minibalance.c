#include "sys.h"
/**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/


**************************************************************************/
//! ʹ��pid�ĵĺ��ĳ������control.c  -->  int TIM1_UP_IRQHandler(void) ����ʱ����ʱ�жϣ�

u8 Flag_Stop = 0, delay_50, delay_flag, Flash_Send;														 // ֹͣ��־λ 50ms��׼��ʱ��־λ
int Encoder, Target_Position = 10000, Target_Velocity = 10;												 // ���������������
int Moto;																								 // ���PWM���� Ӧ��Motor�� ��Moto�¾�
int Voltage;																							 // ��ص�ѹ������صı���
float Position_KP = 120, Position_KI = 0.1, Position_KD = 500, Velocity_KP = 20, Velocity_KI = 30;		 // PIDϵ��
float Amplitude_PKP = 20, Amplitude_PKI = 0.1, Amplitude_PKD = 25, Amplitude_VKP = 2, Amplitude_VKI = 3; // PID������ز���
float Menu_PID = 1, Menu_MODE = 1;
u16 PID_Parameter[10], Flash_Parameter[10]; // Flash�������
u8 Flag_OLED;
int main(void)
{
	Stm32_Clock_Init(9);		//=====ϵͳʱ������
	delay_init(72);				//=====��ʱ��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE); //=====�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);		//=====��SWD�ӿ� �������������SWD�ӿڵ���
	delay_ms(1000);				//=====��ʱ�������ȴ�ϵͳ�ȶ�
	delay_ms(1000);				//=====��ʱ�������ȴ�ϵͳ�ȶ� ��2s
	LED_Init();					//=====��ʼ���� LED ���ӵ�Ӳ���ӿ�
	EXTI_Init();				//=====������ʼ��(�ⲿ�жϵ���ʽ)
	OLED_Init();				//=====OLED��ʼ��
	uart_init(72, 128000);		//=====��ʼ������1

	MiniBalance_PWM_Init(7199, 0); //=====��ʼ��PWM 10KHZ�������������
	if (PAin(12) == 0)
		TIM2->CNT = 0;
	Encoder_Init_TIM4();   //=====��ʼ����������TIM2�ı������ӿ�ģʽ��
	Baterry_Adc_Init();	   //=====��ص�ѹģ�����ɼ���ʼ��
	Flash_Read();		   //=====��Flash�����ȡ����
	Timer1_Init(99, 7199); //=====��ʱ�жϳ�ʼ��
	while (1)
	{
		DataScope();	//===��λ��
		delay_flag = 1; //===50ms�жϾ�׼��ʱ��־λ
		oled_show();	//===��ʾ����
		while (delay_flag)
			;				 //===50ms�жϾ�׼��ʱ  ��Ҫ�ǲ�����ʾ��λ����Ҫ�ϸ��50ms��������
		if (Flash_Send == 1) // д��PID������Flash,�ɰ������Ƹ�ָ��
		{
			Flash_Write();	// �Ѳ���д�뵽Flash
			Flash_Send = 0; // ��־λ����
			Flag_OLED = 1;	// ��ʾ��־λ��1 ����ʾ��������ʾData Is Saved������
		}
	}
}
