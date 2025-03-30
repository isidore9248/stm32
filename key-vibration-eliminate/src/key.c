#include "key.h"

void Key_MonitoringData_Init(struct KEY_Monitoring_Data *Key);
void Key_SetGPIO(struct KEY_Monitoring_Data *Key, uint16_t KEY_Pin_in, GPIO_TypeDef* KEY_Port_in, uint32_t KEY_Rcc_in);
void KEY_GPIO_Init(uint16_t KEY_Pin_in, GPIO_TypeDef* KEY_Port_in, uint32_t KEY_Rcc_in);
void TIM4_KeyScan_Init(u16 arr, u16 psc);

void KEY_Init(struct KEY_Monitoring_Data *Key,
	uint16_t KEY_Pin_in,
	GPIO_TypeDef* KEY_Port_in,
	uint32_t KEY_Rcc_in)
{
	Key_MonitoringData_Init(Key); // 按键状态数据
	Key_SetGPIO(Key, KEY_Pin_in, KEY_Port_in, KEY_Rcc_in);
	KEY_GPIO_Init(Key->KEY_Pin, Key->KEY_Port, Key->KEY_Rcc);
	TIM4_KeyScan_Init(1000 - 1, 72 - 1); // 72MHZ/72=1MHZ 1ms时基
}

uint8_t KEY_IsTrgered(struct KEY_Monitoring_Data *Key, uint8_t Read_data)
{
	return (Read_data & (Read_data ^ Key->Cont));
}

void KEY_Scan(struct KEY_Monitoring_Data *Key)
{
	static uint8_t KEY_state; // 记录是否双击
	static uint8_t KEY_num;
	uint8_t Read_data;

	//通过异或操作将按键按下时的值变为1，未按下时的值变为0。
	Read_data = Key->Read_KEY_State(Key->KEY_Port, Key->KEY_Pin) ^ 0x01;
	Key->Trg = KEY_IsTrgered(Key, Read_data); // 检测是否改变
	Key->Cont = Read_data; //更新按键的连续状态，保存当前按键状态以供下次扫描时使用。

	//第一次按下不会进入任何一个判断

	//持续按下才会使num++
	if ((Key->Trg == 0) && (Key->Cont == 1))	//按下未松开
	{
		if (Key->num < 100) { Key->num++; }
		if (Key->num == 100) // 满足长按
		{
			Key->state = LONG_CLICKED;
		}
	}

	if ((Key->Trg == 0) && (Key->Cont == 0)) // 松手或者未按下
	{
		if ((Key->num < 50) && (Key->num > 1)) // 检测到按键按下超过40ms且小于1s
		{
			if (KEY_state == 0)
			{
				KEY_state = 1;
				KEY_num = 0;
			}
			else
			{
				// 双击
				if (KEY_num > 8)  { Key->state = DOUBLE_CLICKED; }
				KEY_num = 0;
				KEY_state = 0;
			}
		}
		Key->num = 0;
	}

	// 短按
	if (KEY_state == 1)
	{
		KEY_num++;
		if (KEY_num == 25) // 0.5s
		{
			KEY_num = 0;
			KEY_state = 0;
			Key->state = CLICKED;
		}
	}
}

enum KEY_STATE KEY_GetState(struct KEY_Monitoring_Data *Key)
{
	return	Key->state ;
};

void KEY_CleanState(struct KEY_Monitoring_Data *Key)
{
	Key->state = NO_CLICKED;
}

static u8 KEY_SCAN_FLAG = 0;

bool KEY_Can_Scan(void)
{
	if (KEY_SCAN_FLAG == 1) { return true ; }
	return false ;
}

void KEY_CleanScanFlag(void)
{
	KEY_SCAN_FLAG = 0;
}

void KEY_Scan_TIMProcess(void)
{
	static u8 KEY_Scan_Counter = 0;
	KEY_Scan_Counter++;
	if (KEY_Scan_Counter == 20) // 20ms
	{
		KEY_Scan_Counter = 0;
		KEY_SCAN_FLAG = 1;
	}
}

void Key_MonitoringData_Init(struct KEY_Monitoring_Data *Key) // 按键状态数据
{
	Key->num = 0;
	Key->Trg = 0;
	Key->Cont = 0;
	Key->state = 0;
}

void Key_SetGPIO(struct KEY_Monitoring_Data *Key,
	uint16_t KEY_Pin_in,
	GPIO_TypeDef* KEY_Port_in,
	uint32_t KEY_Rcc_in)
{
	Key->KEY_Pin = KEY_Pin_in;
	Key->KEY_Port = KEY_Port_in;
	Key->KEY_Rcc = KEY_Rcc_in;
	Key->Read_KEY_State = GPIO_ReadInputDataBit;
}

void KEY_GPIO_Init(uint16_t KEY_Pin_in, GPIO_TypeDef* KEY_Port_in, uint32_t KEY_Rcc_in)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(KEY_Rcc_in, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_Pin_in;
	GPIO_Init(KEY_Port_in, &GPIO_InitStructure);
}

void TIM4_KeyScan_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // 使能TIM4时钟

	TIM_TimeBaseStructure.TIM_Period = arr; // 自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; // 定时器分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); // 初始化TIM4

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // 允许更新中断
	TIM_Cmd(TIM4, ENABLE); // 使能TIM4

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); // 初始化NVIC
}