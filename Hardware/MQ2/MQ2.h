#ifndef __MQ2_H
#define __MQ2_H

#define RL  5.1 //RL����ֵ
#define CAL_PPM 20//У׼������ppm��ֵ


void MQ2_Init(void);
float MQ2_GetPPM(void);
void TIM3_Init(void);
void TIM3_IRQHandler(void);

#endif
