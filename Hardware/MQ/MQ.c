#include "stm32f10x.h"                  // Device header
#include "MQ.h"
#include "Delay.h"
#include "math.h"

float R0=20;//元件在洁净空气中的阻值
uint16_t AD_Value[2];
uint16_t times;

void MQ_Init()//MQ2初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_239Cycles5);
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}



static uint16_t Get_ADC_Average(uint8_t times)//利用AD返回值做计算，times为次数
{
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++){
		temp_val+=AD_Value[0];
		Delay_ms(5);
	}
	return temp_val/times;
}


float MQ2_GetPPM()
{
	uint16_t x;
	x=Get_ADC_Average(30);
	float Vr1=3.3f * x / 4096.f;
	Vr1=((float)((int)((Vr1+0.005)*100)))/100;
	float RS=(3.3f-Vr1)/ Vr1 * RL;
	if(times<6) // 获取系统执行时间，3s前进行校准
    {
		  R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
    } 
	float ppm=613.9f * pow(RS/R0,-2.074f);
	return ppm;
}

void TIM3_Init(void)
{	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitsture;
	NVIC_InitTypeDef NVIC_Initstructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitsture.TIM_Period=5000-1;
	TIM_TimeBaseInitsture.TIM_Prescaler=7200-1;
	TIM_TimeBaseInitsture.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitsture.TIM_ClockDivision=TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitsture);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_Initstructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority=12;
	NVIC_Initstructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Initstructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_Initstructure);
	
	TIM_Cmd(TIM3,ENABLE);
}

static uint16_t ADC2_Average_Data(uint8_t CO_READ_TIMES)
{
	uint16_t temp_val=0;
	uint8_t t;
	for(t=0;t<CO_READ_TIMES;t++)	//#define CO_READ_TIMES	10	定义烟雾传感器读取次数,读这么多次,然后取平均值 
	{
		temp_val+=AD_Value[1];
		Delay_ms(5);
	}
	temp_val/=CO_READ_TIMES;//得到平均值
    return (uint16_t)temp_val;//返回算出的ADC平均值
}


float CO_Get_Vol()
{
	uint16_t adc_value = 0;//这是从MQ-7传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
	float voltage = 0;//MQ-7传感器模块的电压输出，与一氧化碳的浓度成正比
	
	adc_value = ADC2_Average_Data(30);
	Delay_ms(5);
    voltage  = (3.3/4096.0)*(adc_value);
	
	return voltage;
}


float MQ7_GetPPM()
{
	float RS = (3.3f - CO_Get_Vol()) / CO_Get_Vol() * C0_RL;
	float ppm = 98.322f * pow(RS/CO_R0, -1.458f);
	return  ppm;
}

void TIM3_IRQHandler()
{	
	 if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)//判断状态
	 { 
		 TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除中断待处理位
		 times++;
	 }
}
