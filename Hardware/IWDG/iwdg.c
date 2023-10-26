#include "stm32f10x.h"                  // Device header
#include "iwdg.h"


void IWDG_Init(uint8_t prer, uint16_t rlr)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(prer);
  IWDG_SetReload(rlr);
  IWDG_ReloadCounter();
  IWDG_Enable();
}

void IWDG_Feed(void)
{
  IWDG_ReloadCounter();
}
