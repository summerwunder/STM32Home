#include "stm32f10x.h"                  // Device header
#include "USART.h"
uint8_t RxFlag;
char RxData[MAX_RX_DATA_LENGTH];
void USART1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*初始化USART*/
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    
    /*设置中断*/
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1,ENABLE);
}

void USART_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i ++)
    {
        USART_SendByte(String[i]);
    }
}

int fputc(int ch, FILE *f)
{
    USART_SendByte(ch);
    return ch;
}

void USART1_IRQHandler(void)
{
    static uint8_t RxState=0;
    static uint8_t RxNum = 0;
    static char Prefix[] = "+MQTTSUBRECV:"; // 前缀字符串
    static uint8_t PrefixLen = sizeof(Prefix) - 1; // 前缀长度
    
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t TmpData=USART_ReceiveData(USART1);
        
        if(RxState==0)
        {
            if(TmpData == Prefix[RxNum] && RxFlag == 0)
            {
                RxNum++;            
                if(RxNum == PrefixLen)
                {
                    RxState = 1;
                    RxNum = 0;
                }
            } 
        }
        else if(RxState==1)
        {
            if(TmpData!='\r')
            {
               RxData[RxNum]=TmpData;
               RxNum++;      
            }
            else
            {
                 RxState=2;
            }
        }
        else if(RxState==2)
        {
            if (TmpData == '\n')
            {
                RxState = 0;
                RxData[RxNum]='\0';
                RxFlag = 1;
                RxNum=0;
            }
        }       
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

