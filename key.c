#include "key.h"

struct KEYdataBase KeyData;
struct KEYscanDataBase KeyScanData;

void KeyDataInit(void) // 按键状态数据
{
    KeyData.KEY1 = NO_CLICKED;
    KeyData.KEY2 = NO_CLICKED;
}

void KeyScanDataInit(void) // 按键扫描处理数据
{
    KeyScanData.KEY1_Cont = 0;
    KeyScanData.KEY1_num = 0;
    KeyScanData.KEY1_Trg = 0;

    KeyScanData.KEY2_Cont = 0;
    KeyScanData.KEY2_num = 0;
    KeyScanData.KEY2_Trg = 0;
}

void KEY_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN;

    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

void TIM4_KeyScan_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // 使能TIM4时钟

    TIM_TimeBaseStructure.TIM_Period = arr;    // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = psc; // 定时器分频
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); // 初始化TIM4

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // 允许更新中断
    TIM_Cmd(TIM4, ENABLE);                     // 使能TIM4

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC
}

void KEY_Init(void)
{
    KeyScanDataInit();
    KeyDataInit();
    KEY_GPIO_Init();
    TIM4_KeyScan_Init(1000 - 1, 72 - 1); // 72MHZ/72=1MHZ 1ms时基
}

void KEY_Scan(void)
{
    static uint8_t KEY1_state; // 记录是否双击
    static uint8_t KEY1_num;

    uint8_t Read_data;

    Read_data = KEY1_SATE() ^ 0x01;
    KeyScanData.KEY1_Trg = Read_data & (Read_data ^ KeyScanData.KEY1_Cont);
    KeyScanData.KEY1_Cont = Read_data;

    if ((KeyScanData.KEY1_Trg == 0) && (KeyScanData.KEY1_Cont == 0)) // 松手或者未按下
    {
        if ((KeyScanData.KEY1_num < 50) && (KeyScanData.KEY1_num > 1)) // 检测到按键按下超过40ms且小于1s
        {
            if (KEY1_state == 0)
            {
                KEY1_state = 1;
                KEY1_num = 0;
            }
            else
            {
                if (KEY1_num > 8) // 双击
                {
                    KeyData.KEY1 = DOUBLE_CLICKED;
                }
                KEY1_num = 0;
                KEY1_state = 0;
            }
        }
        KeyScanData.KEY1_num = 0;
    }
    if (KEY1_state == 1) // 短按
    {
        KEY1_num++;
        if (KEY1_num == 25) // 0.5s
        {
            KEY1_num = 0;
            KEY1_state = 0;
            KeyData.KEY1 = CLICKED;
        }
    }

    if ((KeyScanData.KEY1_Trg == 0) && (KeyScanData.KEY1_Cont == 1))
    {
        if (KeyScanData.KEY1_num < 100)
            KeyScanData.KEY1_num++;
        if (KeyScanData.KEY1_num == 100) // 满足长按
        {
            KeyData.KEY1 = LONG_CLICKED;
            // KeyScanData.KEY1_num++;
        }
    }
}
