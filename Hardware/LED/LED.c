#include "stm32f10x.h"                  // Device header
#include "LED.h"

void LED_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void LED_OFF(){
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}
void LED_ON(){
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
}
