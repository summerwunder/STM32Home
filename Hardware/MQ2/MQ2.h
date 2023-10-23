#ifndef __MQ2_H
#define __MQ2_H

#define RL  5 //RL的阻值
#define CAL_PPM 20//校准环境中ppm的值
extern uint16_t AD_Value[2];

void MQ2_Init(void);
float MQ2_GetPPM(void);
void TIM3_Init(void);
void TIM3_IRQHandler(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);
#endif
