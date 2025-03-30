#ifndef __KEY_H
#define __KEY_H

#include "STM32Headfile.h"

enum KEY_STATE
{
	NO_CLICKED     = 0,
	CLICKED,
	DOUBLE_CLICKED,
	LONG_CLICKED,
};

struct KEY_Monitoring_Data
{
	uint8_t Trg;
	uint8_t Cont;
	uint8_t num;
	volatile enum KEY_STATE state;

	uint16_t KEY_Pin;
	GPIO_TypeDef* KEY_Port;
	uint32_t KEY_Rcc;

	uint8_t(*Read_KEY_State)(GPIO_TypeDef*, uint16_t);
};

void KEY_Init(struct KEY_Monitoring_Data *Key,
	uint16_t KEY_Pin_in,
	GPIO_TypeDef* KEY_Port_in,
	uint32_t KEY_Rcc_in);

void KEY_Scan(struct KEY_Monitoring_Data *Key);
enum KEY_STATE KEY_GetState(struct KEY_Monitoring_Data *Key);

void KEY_CleanState(struct KEY_Monitoring_Data *Key);

bool KEY_Can_Scan(void);
void KEY_CleanScanFlag(void);

void KEY_Scan_TIMProcess(void);

#endif