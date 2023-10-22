#ifndef __FAN_H
#define __FAN_H
#define FAN_HIGH 3
#define FAN_MID 2
#define FAN_LOW 1
#define FAN_OFF 0 

void FAN_Init(void);
void FAN_Setcompare2(uint16_t Compare);
void FAN_SetAngle(float Angle);
void FAN_Speed(u16 t);
#endif