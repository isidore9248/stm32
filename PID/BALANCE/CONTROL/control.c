#include "control.h"
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
/**************************************************************************
函数功能：所有的控制代码都在这里面
		  TIM1控制的定时中断
**************************************************************************/
u8 Flag_Velocity = 1;

//! 使用pid的地方
int TIM1_UP_IRQHandler(void)
{
	if (TIM1->SR & 0X0001) // 5ms定时中断
	{
		TIM1->SR &= ~(1 << 0); //===清除定时器1中断标志位

		// * 位置控制或者速度控制
		if (Menu_MODE == 1)
		{
			Encoder = Read_Position(4);					   //===更新位置信息
			Moto = Position_PID(Encoder, Target_Position); //===位置PID控制器
		}
		else if (++Flag_Velocity > 2) //===速度控制20ms一次，一般建议10ms，因为这里使用USB供电，速度比较慢，20ms可以延长获取速度的单位时间，提高数值
		{
			Flag_Velocity = 1;
			Encoder = Read_Velocity(4); //===更新速度信息
			if (Encoder < -50)
				Encoder = -50;
			Moto = Incremental_PI(Encoder, Target_Velocity); //===速度PI控制器
		}
		// * 位置控制或者速度控制 --- end

		// * 实现50ms的精准延时，用于主函数中的波形显示和数据传输。
		if (delay_flag == 1)
		{
			if (++delay_50 == 5)
				delay_50 = 0, delay_flag = 0; //===给主函数提供50ms的精准延时
		}

		Xianfu_Pwm(); //===PWM限幅

		//* 电机停止判断
		if (Flag_Stop == 0)
			Set_Pwm(Moto); //===赋值给PWM寄存器
		else
			Set_Pwm(0); //===赋值给PWM寄存器

		//* 其余设置
		Led_Flash(100);				  //===LED闪烁指示系统正常运行
		Key();						  //===扫描按键变化
		Voltage = Get_battery_volt(); //===采集供电电压
	}
	return 0;
}
/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto)
{
	if (moto < 0)
		AIN2 = 1, AIN1 = 0;
	else
		AIN2 = 0, AIN1 = 1;
	PWMA = myabs(moto);
}

/**************************************************************************
函数功能：限制PWM赋值
入口参数：无
返回  值：无
**************************************************************************/

//*操作电机的pwm值（全局变量）
void Xianfu_Pwm(void)
{
	int Amplitude = 7100; //===PWM满幅是7200 限制在7100
	if (Moto < -Amplitude)
		Moto = -Amplitude;
	if (Moto > Amplitude)
		Moto = Amplitude;
}
/**************************************************************************
函数功能：按键修改运行状态
入口参数：无
返回  值：无
**************************************************************************/
void Key(void)
{
	int tmp, tmp2, Position_Amplitude = 260, Velocity_Amplitude = 15;
	tmp = click_N_Double(50);
	if (Menu_MODE == 1) // 改变运行位置
	{
		if (tmp == 1)
			Target_Position += Position_Amplitude;
		if (tmp == 2)
			Target_Position -= Position_Amplitude;
	}
	else // 改变运行速度
	{
		if (tmp == 1)
			Target_Velocity += Velocity_Amplitude;
		if (tmp == 2)
			Target_Velocity -= Velocity_Amplitude;
	}
	if (Target_Velocity > 50)
		Target_Velocity = 50; // 速度最大值限幅
	if (Target_Velocity < -50)
		Target_Velocity = -50; // 速度最小值限幅
	tmp2 = Long_Press();
	if (tmp2 == 1)
		Flash_Send = 1; // 长按把参数写入Flash实现掉电保存
}

/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{
	int temp;
	if (a < 0)
		temp = -a;
	else
		temp = a;
	return temp;
}
/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差
e(k-1)代表上一次的偏差  以此类推
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI(int Encoder, int Target)
{
	static float Bias, Pwm, Last_bias;
	Bias = Encoder - Target;									  // 计算偏差
	Pwm += Velocity_KP * (Bias - Last_bias) + Velocity_KI * Bias; // 增量式PI控制器
	Last_bias = Bias;											  // 保存上一次偏差
	return Pwm;													  // 增量输出
}
/**************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM
根据位置式离散PID公式
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差
e(k-1)代表上一次的偏差
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
int Position_PID(int Encoder, int Target)
{
	static float Bias, Pwm, Integral_bias, Last_Bias;
	Bias = Encoder - Target;																   // 计算偏差
	Integral_bias += Bias;																	   // 求出偏差的积分
	Pwm = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias); // 位置式PID控制器
	Last_Bias = Bias;																		   // 保存上一次偏差
	return Pwm;																				   // 增量输出
}
