/*
 * @Description:
 * @Version: v1.0.0
 * @Author: isidore-chen
 * @Date: 2025-3-30 15:13:54
 * @Copyright: Copyright (c) 2024 CAUC
 */

/*
 * 修改地方：
 * 1、nrf24l01.h 引脚定义
 * 所有引脚均为软件模拟，可任意修改
 * 2、nrf24l01.c T_ADDR、R_ADDR接收发送地址
 * note：电脑端广播地址一般为 0xFF, 0xFF, 0xFF, 0xFF, 0xFF （40位）
 * 根据需要修改
 * 3、接收数据在recv_test()函数的	 NRF24L01_Cmd TargetValue; //NRF24L01接收到的数据结构体
 * NRF24L01_CmdValue_Init(&TargetValue); // 初始化接收数据结构体
 * 4、指令数组使用CAUC_A、CAUC_C、CAUC_D，匹配函数为 is_buf_match()
 */

 #include "STM32Headfile.h"
 #include "OLED.h"
 #include "nrf24l01.h"
 #include "Delay.h"
 
 void send_test();
 void rec_test();
 
/**
 * @brief 判断缓冲区内容是否匹配
 *
 * @param buf 缓冲区指针
 * @param target 目标数组指针
 * @param len 比较的长度
 * @return true 如果匹配
 * @return false 如果不匹配
 */
bool is_buf_match(uint8_t *buf, uint8_t *target, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		if (buf[i] != target[i])
		{
			return false;
		}
	}
	return true;
}
 
 // 上位机发送的数据
 uint8_t CAUC_A[6] = { 0x43, 0x41, 0x55, 0x43, 0x2D, 0x41 }; // 目标数组
 uint8_t CAUC_C[6] = { 0x43, 0x41, 0x55, 0x43, 0x2D, 0x43 }; // 目标数组
 uint8_t CAUC_D[6] = { 0x43, 0x41, 0x55, 0x43, 0x2D, 0x44 }; // 目标数组
 
 int main(void)
 {
	 rec_test();
	 //send_test();
 }
 
 /**
  * @brief 发送测试函数
  *
  * 初始化OLED，初始化NRF24L01，发送数据
  */
 void send_test()
 {
	 OLED_Init();
	 NRF24L01_Init();
	 uint8_t Buf[32] = { 0x66, 0x66, 0x66, 0x66, 0x66, 0x66 };
	 OLED_ShowString(1, 1, "Init_OK");
 
	 if (NRF24L01_Check())
	 {
		 OLED_ShowString(2, 1, "read_ok");
	 }
	 else
	 {
		 OLED_ShowString(2, 1, "Init_Fail");
		 while (1) ;
	 }
 
	 while (1)
	 {
		 NRF24L01_Send(Buf);
	 }
 }
 
 /**
  * @brief 接收测试函数
  *
  * 初始化OLED，初始化NRF24L01，接收数据并显示
  */
 
 void rec_test()
 {
	 OLED_Init();
 
	 NRF24L01_Init();
	 //uint8_t addr[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	 //NRF24L01_Set_RxAddr(addr, 5);
	 //NRF24L01_Set_TxAddr(addr, 5);
 
	 // 检测NRF24L01是否正常连接
	 if (NRF24L01_Check())
	 {
		 OLED_ShowString(4, 1, "Init_ok");
	 }
	 else
	 {
		 OLED_ShowString(4, 1, "Init_Fail");
		 while (1) ;
	 }
 
	 NRF24L01_Cmd TargetValue; //NRF24L01接收到的数据结构体
	 NRF24L01_CmdValue_Init(&TargetValue);
	 while (1)
	 {
		 if (NRF24L01_R_IRQ() == 0)
		 {
			 NRF24L01_Receive(&TargetValue);
		 }
 
		 OLED_ShowHexNum(1, 1, TargetValue.NRF24L01_CmdValue[0], 2);
		 OLED_ShowHexNum(1, 4, TargetValue.NRF24L01_CmdValue[1], 2);
		 OLED_ShowHexNum(2, 1, TargetValue.NRF24L01_CmdValue[2], 2);
		 OLED_ShowHexNum(2, 4, TargetValue.NRF24L01_CmdValue[3], 2);
		 OLED_ShowHexNum(3, 1, TargetValue.NRF24L01_CmdValue[4], 2);
		 OLED_ShowHexNum(3, 4, TargetValue.NRF24L01_CmdValue[5], 2);
 
		 if (is_buf_match(TargetValue.NRF24L01_CmdValue, CAUC_A, 6))
		 {
			 // 匹配到CAUC-A
			 OLED_ShowString(4, 1, "CAUC-A");
		 }
		 else if (is_buf_match(TargetValue.NRF24L01_CmdValue, CAUC_C, 6))
		 {
			 // 匹配到CAUC-C
		 }
		 else if (is_buf_match(TargetValue.NRF24L01_CmdValue, CAUC_D, 6))
		 {
			 // 匹配到CAUC-D
		 }
		 else
		 {
			 // 未匹配到
		 }
	 }
 }