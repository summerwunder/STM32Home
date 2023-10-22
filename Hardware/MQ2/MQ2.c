#include "stm32f10x.h"                  // Device header
#include "MQ2.h"
#include "Delay.h"
#include "math.h"

float R0=6;//元件在洁净空气中的阻值
uint16_t AD_Value[4];
uint16_t times;

void MQ2_Init()//MQ2初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
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
	

u16 Get_ADC_Average(u8 times)//利用AD返回值做计算，times为次数
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++){
		temp_val+=AD_Value[0];
		Delay_ms(5);
	}
	return temp_val/times;
}
void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		times++;
	}
}

float MQ2_GetPPM()
{
	u16 x;
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

void TIM3_Init()
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
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_Initstructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Initstructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_Initstructure);
	
	TIM_Cmd(TIM3,ENABLE);
}

