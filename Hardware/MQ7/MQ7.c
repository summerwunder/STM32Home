#include "stm32f10x.h"                  // Device header
#include "MQ7.h"
#include "MQ2.h"
#include "Delay.h"
#include "Math.h"
//extern uint8_t AD_Value[2];
void MQ7_Init(){
	MQ2_Init();
}
u16 ADC2_Average_Data(u8 CO_READ_TIMES)
{
	u16 temp_val=0;
	u8 t;
	for(t=0;t<CO_READ_TIMES;t++)	//#define CO_READ_TIMES	10	定义烟雾传感器读取次数,读这么多次,然后取平均值
 
	{
		temp_val+=AD_GetValue(ADC_Channel_6);	//读取ADC值
		Delay_ms(5);
	}
	temp_val/=CO_READ_TIMES;//得到平均值
    return (u16)temp_val;//返回算出的ADC平均值
}


float CO_Get_Vol()
{
	u16 adc_value = 0;//这是从MQ-7传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
	float voltage = 0;//MQ-7传感器模块的电压输出，与一氧化碳的浓度成正比
	
	adc_value = ADC2_Average_Data(30);
	Delay_ms(5);
    voltage  = (3.3/4096.0)*(adc_value);
	
	return voltage;
}


float MQ7_GetPPM()
{
	float RS = (3.3f - CO_Get_Vol()) / CO_Get_Vol() * RL;
	float ppm = 98.322f * pow(RS/CO_R0, -1.458f);
	return  ppm;
}

