#ifndef __MQ_H
#define __MQ_H

#define RL  5 //RL����ֵ
#define CAL_PPM 20//У׼������ppm��ֵ
#define CO_CAL_PPM  10  // У׼������PPMֵ
#define C0_RL	     10  // RL��ֵ
#define CO_R0	     16  // R0��ֵ

void MQ_Init(void);
extern uint16_t AD_Value[2];
void TIM3_Init(void);

float MQ2_GetPPM(void);
float MQ7_GetPPM(void);

#endif