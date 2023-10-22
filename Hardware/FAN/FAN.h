#ifndef __FAN_H
#define __FAN_H
#define HIGH 3
#define MID 2
#define LOW 1
#define OFF 0 

void FAN_Init(void);
void FAN_Setcompare2(uint16_t Compare);
void FAN_SetAngle(float Angle);
void FAN_Speed(u16 t);
#endif