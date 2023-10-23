#ifndef __MQ_H
#define __MQ_H

#define RL  5 //RL的阻值
#define CAL_PPM 20//校准环境中ppm的值
#define CO_CAL_PPM  10  // 校准环境中PPM值
#define C0_RL	     10  // RL阻值
#define CO_R0	     16  // R0阻值

void MQ_Init(void);
extern uint16_t AD_Value[2];
void TIM3_Init(void);

float MQ2_GetPPM(void);
float MQ7_GetPPM(void);

#endif