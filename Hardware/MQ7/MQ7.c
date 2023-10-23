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
	for(t=0;t<CO_READ_TIMES;t++)	//#define CO_READ_TIMES	10	��������������ȡ����,����ô���,Ȼ��ȡƽ��ֵ
 
	{
		temp_val+=AD_GetValue(ADC_Channel_6);	//��ȡADCֵ
		Delay_ms(5);
	}
	temp_val/=CO_READ_TIMES;//�õ�ƽ��ֵ
    return (u16)temp_val;//���������ADCƽ��ֵ
}


float CO_Get_Vol()
{
	u16 adc_value = 0;//���Ǵ�MQ-7������ģ���ѹ�����ADCת���л�õ�ԭʼ����ֵ����ֵ�ķ�ΧΪ0��4095����ģ���ѹ��ʾΪ����ֵ
	float voltage = 0;//MQ-7������ģ��ĵ�ѹ�������һ����̼��Ũ�ȳ�����
	
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

