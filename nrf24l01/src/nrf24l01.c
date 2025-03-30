#include "STM32Headfile.h"
#include "nrf24l01.h"
#include "nrf24l01_config.h"
#include "Delay.h"

#define  ADDR_WIDTH		5
#define  PAYLOAD_WIDTH  32

static uint8_t T_ADDR[ADDR_WIDTH] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; //本机地址
static uint8_t R_ADDR[ADDR_WIDTH] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; //目标地址

static uint8_t NRF24L01_RecvBuf[32] = { 0 };

/**
 * @brief 设置发送地址
 * @param addr 发送地址
 * @param len 地址长度
 */
void NRF24L01_Set_TxAddr(uint8_t *addr, uint8_t len)
{
	if (len > ADDR_WIDTH) len = ADDR_WIDTH; // 限制地址长度为5字节
	for (uint8_t i = 0; i < len; i++)
	{
		T_ADDR[i] = addr[i];
	}
	NRF24L01_W_Buf(W_REGISTER + TX_ADDR, T_ADDR, ADDR_WIDTH); // 更新发送地址
}

/**
 * @brief 设置接收地址
 * @param addr 接收地址
 * @param len 地址长度
 */
void NRF24L01_Set_RxAddr(uint8_t *addr, uint8_t len)
{
	if (len > ADDR_WIDTH) len = ADDR_WIDTH; // 限制地址长度为5字节
	for (uint8_t i = 0; i < len; i++)
	{
		R_ADDR[i] = addr[i];
	}
	NRF24L01_W_Buf(W_REGISTER + RX_ADDR_P0, R_ADDR, ADDR_WIDTH); // 更新接收地址
}

/**
 * @brief 设置MOSI引脚电平
 * @param Value 电平值
 */
void NRF24L01_W_MOSI(uint8_t Value)
{
	GPIO_WriteBit(MOSI_Port, MOSI_Pin, (BitAction)Value);
}

/**
 * @brief 设置SCK引脚电平
 * @param Value 电平值
 */
void NRF24L01_W_SCK(uint8_t Value)
{
	GPIO_WriteBit(SCK_Port, SCK_Pin, (BitAction)Value);
}

/**
 * @brief 设置CSN引脚电平
 * @param Value 电平值
 */
void NRF24L01_W_CSN(uint8_t Value)
{
	GPIO_WriteBit(CSN_Port, CSN_Pin, (BitAction)Value);
}

/**
 * @brief 设置CE引脚电平
 * @param Value 电平值
 */
void NRF24L01_W_CE(uint8_t Value)
{
	GPIO_WriteBit(CE_Port, CE_Pin, (BitAction)Value);
}

/**
 * @brief 读取IRQ引脚电平
 * @return IRQ引脚电平值
 */
uint8_t NRF24L01_R_IRQ(void)
{
	return GPIO_ReadInputDataBit(IRQ_Port, IRQ_Pin);
}

/**
 * @brief 读取MISO引脚电平
 * @return MISO引脚电平值
 */
uint8_t NRF24L01_R_MISO(void)
{
	return GPIO_ReadInputDataBit(MISO_Port, MISO_Pin);
}

/**
 * @brief 初始化NRF24L01引脚
 */
void NRF24L01_Pin_Init(void)
{
	RCC_APB2PeriphClockCmd(nrf2401l_RCC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	// 配置成推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOSI_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MOSI_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SCK_Pin;
	GPIO_Init(SCK_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = CSN_Pin;
	GPIO_Init(CSN_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = CE_Pin;
	GPIO_Init(CE_Port, &GPIO_InitStructure);

	// 配置成上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = IRQ_Pin;
	GPIO_Init(IRQ_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = MISO_Pin;
	GPIO_Init(MISO_Port, &GPIO_InitStructure);
}

/**
 * @brief SPI交换一个字节
 * @param Byte 要交换的字节
 * @return 接收到的字节
 */
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte)
{
	uint8_t i, ByteReceive = 0x00;
	for (i = 0; i < 8; i++)
	{
		NRF24L01_W_MOSI(Byte & (0x80 >> i));
		NRF24L01_W_SCK(1);
		if (NRF24L01_R_MISO() == 1)
		{
			ByteReceive = ByteReceive | (0x80 >> i);
		}
		NRF24L01_W_SCK(0);
	}
	return ByteReceive;
}

/**
 * @brief 写寄存器
 * @param Reg 寄存器地址
 * @param Value 要写入的值
 */
void NRF24L01_W_Reg(uint8_t Reg, uint8_t Value)
{
	NRF24L01_W_CSN(0); // 表示选中NRF24L01
	NRF24L01_SPI_SwapByte(Reg); // 交换的第一个字节就是指令
	NRF24L01_SPI_SwapByte(Value); // 交换的第二个字节就是交换的数据
	NRF24L01_W_CSN(1); // 停止选中NRF24L01
}

/**
 * @brief 读寄存器
 * @param Reg 寄存器地址
 * @return 读取的值
 */
uint8_t NRF24L01_R_Reg(uint8_t Reg)
{
	uint8_t Value;
	NRF24L01_W_CSN(0); // 表示选中NRF24L01
	NRF24L01_SPI_SwapByte(Reg); // 交换的第一个字节就是指令
	Value = NRF24L01_SPI_SwapByte(NOP); // 交换的第二个字节就是交换的数据
	NRF24L01_W_CSN(1); // 停止选中NRF24L01
	return Value;
}

/**
 * @brief 写缓冲区
 * @param Reg 寄存器地址
 * @param Buf 缓冲区指针
 * @param Len 缓冲区长度
 */
void NRF24L01_W_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t i;
	NRF24L01_W_CSN(0); // 选中NRF24L01
	NRF24L01_SPI_SwapByte(Reg);
	for (i = 0; i < Len; i++)
	{
		NRF24L01_SPI_SwapByte(Buf[i]);
	}
	NRF24L01_W_CSN(1); // 停止选中NRF24L01
}

/**
 * @brief 读缓冲区
 * @param Reg 寄存器地址
 * @param Buf 缓冲区指针
 * @param Len 缓冲区长度
 */
void NRF24L01_R_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t i;
	NRF24L01_W_CSN(0); // 选中NRF24L01
	NRF24L01_SPI_SwapByte(Reg);
	for (i = 0; i < Len; i++)
	{
		Buf[i] = NRF24L01_SPI_SwapByte(NOP);
	}
	NRF24L01_W_CSN(1); // 停止选中NRF24L01
}

/**
 * @brief 初始化NRF24L01
 */
void NRF24L01_Init(void)
{
	NRF24L01_Pin_Init();

	NRF24L01_W_CE(0);

	NRF24L01_W_Buf(W_REGISTER + TX_ADDR, T_ADDR, ADDR_WIDTH); // 配置发送地址
	NRF24L01_W_Buf(W_REGISTER + RX_ADDR_P0, R_ADDR, ADDR_WIDTH); // 配置接收通道0
	NRF24L01_W_Reg(W_REGISTER + CONFIG, 0x0F); // 配置成接收模式
	NRF24L01_W_Reg(W_REGISTER + EN_AA, 0x01); // 通道0开启自动应答
	NRF24L01_W_Reg(W_REGISTER + RF_CH, 0x00); // 配置通信频率2.4G
	NRF24L01_W_Reg(W_REGISTER + RX_PW_P0, PAYLOAD_WIDTH); // 配置接收通道0接收的数据宽度32字节
	NRF24L01_W_Reg(W_REGISTER + EN_RXADDR, 0x01); // 接收通道0使能
	NRF24L01_W_Reg(W_REGISTER + SETUP_RETR, 0x1A); // 配置580us重发时间间隔,重发10次
	NRF24L01_W_Reg(FLUSH_RX, NOP);

	NRF24L01_W_CE(1);
}

/**
 * @brief 检测NRF24L01是否正常连接
 * @return 连接状态
 */
uint8_t NRF24L01_Check(void)
{
	uint8_t status = NRF24L01_R_Reg(CONFIG);
	return (status != 0xFF); // 如果读取到的值不是0xFF，说明NRF24L01正常连接
}

/**
 * @brief 将接收到的数据复制到目标命令值结构中。
 * 
 * @param Buf 接收到的数据缓冲区，包含要复制的数据。
 * @param TargetValue 目标命令值结构的指针，用于存储复制的数据。
 * 
 * @note 该函数假定缓冲区 Buf 的长度至少为 7 字节（包含索引 1 到 6 的数据）。
 *       数据从 Buf 的索引 1 开始复制到 TargetValue->NRF24L01_CmdValue 的索引 0 到 5。
 */
void NRF24L01_CopyRecvToCmdValue(uint8_t *Buf, NRF24L01_Cmd *TargetValue)
{
	for (int i = 1; i <= 6; i++) { TargetValue->NRF24L01_CmdValue[i - 1] = Buf[i]; }
}

/**
 * @brief 接收数据
 * @param TargetValue 传入传出参数指向NRF24L01_Cmd结构体的指针，用于存储接收到的数据
 */
void NRF24L01_Receive(NRF24L01_Cmd *TargetValue)
{
	uint8_t Status;
	Status = NRF24L01_R_Reg(R_REGISTER + STATUS);
	if (Status & RX_OK)
	{
		memset(NRF24L01_RecvBuf, 0, PAYLOAD_WIDTH);

		NRF24L01_R_Buf(R_RX_PAYLOAD, NRF24L01_RecvBuf, PAYLOAD_WIDTH);
		NRF24L01_W_Reg(FLUSH_RX, NOP);
		NRF24L01_W_Reg(W_REGISTER + STATUS, Status);
		Delay_us(150);
	}
	NRF24L01_CopyRecvToCmdValue(NRF24L01_RecvBuf, TargetValue);
}

/**
 * @brief 发送数据
 * @param Buf 发送缓冲区
 * @return 发送状态
 */

uint8_t NRF24L01_Send(uint8_t *Buf)
{
	uint8_t Status;
	NRF24L01_W_Buf(W_TX_PAYLOAD, Buf, PAYLOAD_WIDTH); // 在发送数据缓存器发送要发送的数据

	NRF24L01_W_CE(0);
	NRF24L01_W_Reg(W_REGISTER + CONFIG, 0x0E);
	NRF24L01_W_CE(1);

	while (NRF24L01_R_IRQ() == 1) // 等待中断
	{
		static int count = 0;
		count++;
	}
	Status = NRF24L01_R_Reg(R_REGISTER + STATUS);

	if (Status & MAX_TX) // 如果发送达到最大次数
	{
		NRF24L01_W_Reg(FLUSH_TX, NOP); // 清除发送数据缓存器
		NRF24L01_W_Reg(W_REGISTER + STATUS, Status); // 中断位写1清除中断
		return MAX_TX;
	}
	if (Status & TX_OK) // 如果发送成功,接收到应答信号
	{
		NRF24L01_W_Reg(W_REGISTER + STATUS, Status); // 清除中断
		return TX_OK;
	}
}

/**
 * @brief 初始化 NRF24L01_Cmd 结构体中的命令值数组。
 * 
 * 此函数将 NRF24L01_Cmd 结构体中的 NRF24L01_CmdValue 数组的所有元素初始化为 0。
 * 
 * @param Cmd 指向 NRF24L01_Cmd 结构体的指针。
 */
void NRF24L01_CmdValue_Init(NRF24L01_Cmd* Cmd)
{
	for (int i = 0; i < 6; i++) { Cmd->NRF24L01_CmdValue[i] = 0; }
}