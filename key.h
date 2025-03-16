#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY1_PORT GPIOA
#define KEY2_PORT GPIOA
#define KEY1_PIN GPIO_Pin_4
#define KEY2_PIN GPIO_Pin_5
#define KEY1_SATE() GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN)
#define KEY2_SATE() GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN)

enum KEY_STATE
{
    NO_CLICKED,
    CLICKED,
    DOUBLE_CLICKED,
    LONG_CLICKED
};

struct KEYdataBase
{
    volatile enum KEY_STATE KEY1; 
    volatile enum KEY_STATE KEY2; 
};

struct KEYscanDataBase
{
    uint8_t KEY1_Trg;
    uint8_t KEY1_Cont;
    uint8_t KEY1_num;

    uint8_t KEY2_Trg;
    uint8_t KEY2_Cont;
    uint8_t KEY2_num;
};

void KEY_Init(void);
void KEY_Scan(void);

#endif