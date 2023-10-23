#ifndef __MQ7_H
#define __MQ7_H


#define CO_CAL_PPM  10  // 校准环境中PPM值
#define C0_RL	     10  // RL阻值
#define CO_R0	     16  // R0阻值

void MQ7_Init(void);
float MQ7_GetPPM(void);
#endif
