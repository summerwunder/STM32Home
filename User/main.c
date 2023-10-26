#include "stm32f10x.h"
#include "OLED.h"
#include "DHT.h"
#include "Delay.h"
#include "string.h"
#include "Buzzer.h"
#include "USART.h"
#include "FAN.h"
#include "ESP8266.h"
#include "MQ.h"
#include "LED.h"
#include "iwdg.h"
/*�������*/
uint8_t alterActive;
#define YES 1
#define NO 0
/*���ڸ�ʽ����ʾ��OLED�����������*/
/*���ڴ���������ʵ������*/
char buffer1[4];
char buffer2[4];
/*���������ݵ�ȫ�ֱ���*/
DHT11_Data_TypeDef DHT11_Data;//��ʪ�ȴ���������
double smokeData;
uint8_t tempData;
uint8_t humiData;
double coData;
/*********************/
/*���ڽ������ݵı���������usart.c*/
extern char RxData[MAX_RX_DATA_LENGTH];    
extern uint8_t RxFlag;
/*������ֵ*/
#define TEMP_MAX_ALTER 35
#define HUMI_MAX_ALTER 80
#define SMOKE_MAX_ALTER 100
#define CO_MAX_ALTER 50
/*����esp8266�������ݵ������ö��*/
typedef enum 
{
    LED_ON_CMD=0,
    LED_OFF_CMD,
    BUZZER_OFF_CMD,
    BUZZER_ON_CMD,
    FAN_OFF_CMD,
    FAN_LOW_CMD,
    FAN_MEDIUM_CMD,
    FAN_HIGH_CMD,
    UNKNOWN_CMD
}CommandType;

CommandType command = UNKNOWN_CMD;  // Ĭ��δ֪����

/**
  * @brief  ���崮�ڽ������ݵ�����
  * @param  ��
  * @retval ��
  */
static void CmdDef() 
{   
    /*����esp8266���ʹ������ݣ�������Ӧ������*/
    if (strstr(RxData, "LED_ON")) 
    {
        /*led����*/
        command = LED_ON_CMD;
    }
    else if (strstr(RxData, "LED_OFF")) 
    {
        command = LED_OFF_CMD;
    } 
    else if (strstr(RxData, "BUZZER_OFF")) 
    {
        command = BUZZER_OFF_CMD;
    } 
    else if (strstr(RxData, "BUZZER_ON")) 
    {
        command = BUZZER_ON_CMD;
    } 
    else if (strstr(RxData, "FAN_OFF")) 
    {
        command = FAN_OFF_CMD;
    } 
    else if (strstr(RxData, "FAN_LOW")) 
    {
        command = FAN_LOW_CMD;
    } 
    else if (strstr(RxData, "FAN_MEDIUM")) 
    {
        command = FAN_MEDIUM_CMD;
    } 
    else if (strstr(RxData, "FAN_HIGH")) 
    {
        command = FAN_HIGH_CMD;
    }
}

/**
  * @brief  ��ȡDHT11����ʪ������
  * @param  ��
  * @retval ��
  */
void DHT11_Show(void)
{
    if(Read_DHT11(&DHT11_Data) == SUCCESS)
    {		
        tempData=DHT11_Data.temp_int;
        humiData=DHT11_Data.humi_int;
        OLED_ShowNum(2, 6, DHT11_Data.humi_int, 2);
        OLED_ShowNum(1, 6, DHT11_Data.temp_int, 2);            
    }
}
/**
  * @brief  ������ֵ�ж��Ƿ���Ҫ����
  * @param  ��
  * @retval YES����Ҫ����
            NO������Ҫ����
  */
static uint8_t isAlter(void)
{
    if(tempData>TEMP_MAX_ALTER||humiData>HUMI_MAX_ALTER
        ||smokeData>SMOKE_MAX_ALTER||coData>CO_MAX_ALTER)
    {        
        return YES;           
    } 
    return NO;
}
/**
  * @brief  ���ڴ��ڽ��յ���CMD���д���
  * @param  ��
  * @retval ��
  */
static void UsartRecTest()
{     
    CmdDef();
    switch (command) 
    {
        case LED_ON_CMD:
            LED_ON();
            break;
        case LED_OFF_CMD:
            LED_OFF();
            break;
        case BUZZER_OFF_CMD:
            Buzzer_OFF();
            break;
        case BUZZER_ON_CMD:
            Buzzer_ON();
            break;
        case FAN_OFF_CMD:
            FAN_Speed(FAN_OFF);
            break;
        case FAN_LOW_CMD:
            FAN_Speed(FAN_LOW);
            break;
        case FAN_MEDIUM_CMD:
            FAN_Speed(FAN_MID);
            break;
        case FAN_HIGH_CMD:
            FAN_Speed(FAN_HIGH);
            break;
        default:
            OLED_ShowString(1, 1, "error");
            break;
    }
    RxFlag = 0;
    memset(RxData, 0, MAX_RX_DATA_LENGTH);
}
/**
  * @brief  �����ʼ��
  * @param  ��
  * @retval ��
  */
static void Periph_Init(void)
{
    /*init*/
    OLED_Init();
    USART1_Init();
	DHT11_GPIO_Config();	
    MQ_Init();
    TIM3_Init();
	LED_Init();
	Buzzer_Init();    
    FAN_Init();
    Buzzer_OFF();    
    ESP8266_Init();
    IWDG_Init(4, 625);
}
/**
  * @brief  ��ʼ��OLED������Ϣ
  * @param  ��
  * @retval ��
  */
static void OLED_ShowInit()
{    
    /*OLED��ʾ��ʼ��*/
    OLED_ShowString(1, 1, "temp:");
    OLED_ShowString(2, 1, "humi:");
    OLED_ShowString(3, 1, "MQ2:");
    OLED_ShowString(4, 1, "MQ7:");    
} 
/**
  * @brief  OLED��ʾ��ֵ
  * @param  ��
  * @retval ��
  */
static void OLED_Show(void)
{
    /*�ò��ֻ�ȡ���������ݲ�����OLED��ʾ��ͬʱ��ֵ��ȫ�ֱ���*/
    DHT11_Show();
    smokeData = MQ2_GetPPM();//��vel�ַ�������ת��Ϊ���ִ洢������
    sprintf(buffer1,"%.2lf",smokeData);
    OLED_ShowString(3, 5, buffer1);
    coData=MQ7_GetPPM();
    sprintf(buffer2,"%.2lf",coData);
    OLED_ShowString(4, 5, buffer2);   
}


int main(void)
{      
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ����� 4      
    Periph_Init();
    OLED_ShowInit();
    while(1)
    {  
        OLED_Show();             
        ESP8266_SendData(humiData,tempData,smokeData,coData);
        if(isAlter()==YES&&alterActive==0)
        {
            LED_ON();
            Buzzer_ON();
            FAN_Speed(FAN_HIGH); 
            alterActive=1;                              
        }
        else if(isAlter()==NO&&alterActive==1)
        {
            LED_OFF();
            Buzzer_OFF();
            FAN_Speed(FAN_OFF); 
            alterActive=0;            
        }
        
        Delay_ms(20); 
        /*���յ�������ʱ����*/
        /*����ʱ�������Ч*/
        if(RxFlag==1&&alterActive==0)
        {
            UsartRecTest();
        }
        // ι�������Ź�
        IWDG_Feed();
    }
   
}

