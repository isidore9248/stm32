#include "stm32f10x.h"
#include "sys.h"
#include "nRF24L01_API.h"
GPIO_InitTypeDef GPIO_InitStructure;

#define LED 			PFout(6)
#define NRF_IRQ   PGin(15)  //IRQ主机数据输入,上拉输入


uchar rece_buf[32];

void NVIC_Configuration(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;
  
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void SPI_GPIO_Init(void)
{
	NVIC_Configuration();
  /* 配置神舟I号LED灯使用的GPIO管脚模式*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOF, ENABLE); /*使能LED灯使用的GPIO时钟*/
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_10|GPIO_Pin_13|GPIO_Pin_15|GPIO_Pin_5;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);  /*GPIO口初始化*/
	GPIO_Init(GPIOC, &GPIO_InitStructure);  /*GPIO口初始化*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);  /*GPIO口初始化*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  /*GPIO口初始化*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);  /*GPIO口初始化*/
}

int main(void)
{	
	SPI_GPIO_Init();	
	GPIO_SetBits(GPIOF,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	GPIO_SetBits(GPIOG,GPIO_Pin_15);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	LED=0;
	while(NRF24L01_Check()); // 等待检测到NRF24L01，程序才会向下执行
	NRF24L01_RT_Init();	
	rece_buf[1]=0xc9;					//上
	rece_buf[2]=0xcf;					//上
	rece_buf[3]=0xba;					//海
	rece_buf[4]=0xa3;					//海
	rece_buf[5]=0xb1;					//宝
	rece_buf[6]=0xa6;					//宝
	rece_buf[7]=0xc7;					//嵌
	rece_buf[8]=0xb6;					//嵌
	rece_buf[9]=0xb5;					//电
	rece_buf[10]=0xe7;				//电
	rece_buf[11]=0xd7;				//子
	rece_buf[12]=0xd3;				//子
	rece_buf[0]=12;						//一共要发送12个字节，rece_buf[0]必须是12！！！！！！
	SEND_BUF(rece_buf);
	while(1)
	{
		if(NRF_IRQ==0)	 	// 如果无线模块接收到数据
		{		
			if(NRF24L01_RxPacket(rece_buf)==0)
			{			   
				if(	rece_buf[1]=='1')		 //第1位以后是收到的命令数据，rece_buf[0]是数据位数长度
					LED=~LED;
				if(	rece_buf[1]=='2')	 	 //第1位以后是收到的命令数据，rece_buf[0]是数据位数长度
						;
			}
		}
		
	}
}

