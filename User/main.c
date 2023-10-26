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
/*用于格式化显示于OLED而定义的数组*/
/*对于传输数据无实际意义*/
char buffer1[4];
char buffer2[4];
/*传感器数据的全局变量*/
DHT11_Data_TypeDef DHT11_Data;//温湿度传感器数据
double smokeData;
uint8_t tempData;
uint8_t humiData;
double coData;
/*********************/
/*串口接收数据的变量，来自usart.c*/
extern char RxData[MAX_RX_DATA_LENGTH];    
extern uint8_t RxFlag;

/*定义esp8266接收数据的命令的枚举*/
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

CommandType command = UNKNOWN_CMD;  // 默认未知命令

/**
  * @brief  定义串口接收数据的命令
  * @param  无
  * @retval 无
  */
static void CmdDef() 
{   
    /*根据esp8266发送串口数据，定义相应的命令*/
    if (strstr(RxData, "LED_ON")) 
    {
        /*led点亮*/
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
  * @brief  获取DHT11的温湿度数据
  * @param  无
  * @retval 无
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
  * @brief  对于串口接收到的CMD进行处理
  * @param  无
  * @retval 无
  */
void UsartRecTest()
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
  * @brief  外设初始化
  * @param  无
  * @retval 无
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
}


int main(void)
{      
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组 4      
    Periph_Init();

    /*OLED显示初始化*/
    OLED_ShowString(1, 1, "temp:");
    OLED_ShowString(2, 1, "humi:");
    OLED_ShowString(3, 1, "MQ2:");
    OLED_ShowString(4, 1, "MQ7:");
    
    while(1)
    {  
        /*该部分获取传感器数据并且在OLED显示，同时赋值给全局变量*/
        DHT11_Show();
        smokeData = MQ2_GetPPM();//将vel字符串类型转换为数字存储到数组
        sprintf(buffer1,"%.2lf",smokeData);
        OLED_ShowString(3, 5, buffer1);
        coData=MQ7_GetPPM();
        sprintf(buffer2,"%.2lf",coData);
        OLED_ShowString(4, 5, buffer2);
              
        //printf("%d %d %.2f %.2f\r\n",tempData,humiData,smokeData,coData);
        ESP8266_SendData(humiData,tempData,smokeData,coData);
        Delay_ms(20);
        /*接收到数据则及时处理*/
        if(RxFlag==1)
        {
            UsartRecTest();
        }
    }
   
}

