#pragma once

#include "STM32Headfile.h"
#include "nrf24l01_config.h"

//通信引脚

/*
 * NRF24L01 双排引脚模块示意图 (俯视图)
 * 排针向下，模块正面朝上
 *        ┌───────────────────────────────┐
 *        │      ─┐  ┌─┐  ┌─────────      │
 *        │       │  │ │  │       		  │
 *        │       │  │ │  │       		  │
 *        │       │  │ │  │       		  │
 *        │		  └──┘ └──┘    			  │
 *        │                               │
 *        │                   ───────	  │
 *		  │					  │ 晶  │	  │
 *        │VCC CSN MOSI IRQ	  │ 振  │	  │
 *        ├───┬───┬───┬───┐   ───────	  │
 *        │ 2 │ 4 │ 6 │ 8 │               │
 *        │   │   │   │   │ 			  │
 *        │ 1 │ 3 │ 5 │ 7 │  			  │
 *        ├───┴───┴───┴───┘ 			  │
 *        │GND  CE SCK MISO  			  │
 *        └───────────────────────────────┘
 *
 */

#define nrf2401l_RCC	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB

#define IRQ_Port   GPIOB
#define CE_Port    GPIOB
#define MISO_Port  GPIOA
#define MOSI_Port  GPIOA
#define SCK_Port   GPIOA
#define CSN_Port   GPIOA

#define IRQ_Pin    GPIO_Pin_1
#define CE_Pin     GPIO_Pin_0
#define MOSI_Pin   GPIO_Pin_7
#define MISO_Pin   GPIO_Pin_6
#define SCK_Pin    GPIO_Pin_5
#define CSN_Pin    GPIO_Pin_4

typedef struct { uint8_t NRF24L01_CmdValue[6]; } NRF24L01_Cmd;

// 外部函数
void NRF24L01_Init(void);
void NRF24L01_CmdValue_Init(NRF24L01_Cmd* Cmd);

uint8_t NRF24L01_Check(void);
void NRF24L01_Set_TxAddr(uint8_t *addr, uint8_t len);
void NRF24L01_Set_RxAddr(uint8_t *addr, uint8_t len);
void NRF24L01_Receive(NRF24L01_Cmd *TargetValue);
uint8_t NRF24L01_Send(uint8_t *Buf);

// 内部函数
void NRF24L01_W_MOSI(uint8_t Value);
void NRF24L01_W_SCK(uint8_t Value);
void NRF24L01_W_CSN(uint8_t Value);
void NRF24L01_W_CE(uint8_t Value);
uint8_t NRF24L01_R_IRQ(void);
uint8_t NRF24L01_R_MISO(void);
void NRF24L01_Pin_Init(void);
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte);
void NRF24L01_W_Reg(uint8_t Reg, uint8_t Value);
uint8_t NRF24L01_R_Reg(uint8_t Reg);
void NRF24L01_W_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len);
void NRF24L01_R_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len);
void NRF24L01_CMdValue_Init(void);
void NRF24L01_CopyRecvToCmdValue(uint8_t *Buf, NRF24L01_Cmd *TargetValue);
