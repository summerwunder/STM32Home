#ifndef __MQ7_H
#define __MQ7_H


#define CO_CAL_PPM  10  // У׼������PPMֵ
#define C0_RL	     10  // RL��ֵ
#define CO_R0	     16  // R0��ֵ

void MQ7_Init(void);
float MQ7_GetPPM(void);
#endif
