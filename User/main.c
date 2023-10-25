#include "stm32f10x.h"
#include "task.h"
#include "OLED.h"
#include "DHT.h"
#include "Delay.h"
#include "string.h"
#include "Buzzer.h"
#include "USART.h"
#include "FAN.h"
#include "ESP8266.h"
#include "event_groups.h"
#include "MQ.h"
#include "LED.h"

char buffer1[4];
char buffer2[4];
DHT11_Data_TypeDef DHT11_Data;//温湿度传感器数据
double smokeData;
uint8_t tempData;
uint8_t humiData;
double coData;
char receivedData[MAX_RX_DATA_LENGTH];    
int num=0;












static void DHT11_Show(void)
{
    if(Read_DHT11(&DHT11_Data) == SUCCESS)
    {		
        OLED_ShowNum(2, 6, DHT11_Data.humi_int, 2);
        OLED_ShowNum(1, 6, DHT11_Data.temp_int, 2);            
    }
}


void sensor_task(void *pvParameters)
{
    while(1)
    {  
        if(Read_DHT11(&DHT11_Data) == SUCCESS)
        {
            tempData=DHT11_Data.temp_int;
            humiData=DHT11_Data.humi_int;
        }
        BaseType_t xReturn = pdPASS;
        xReturn = xSemaphoreGive(xSemaphore);//给出二值信号量     
        if( xReturn == pdTRUE )
		{	       
            printf("%d %d %f %f OK\r\n",tempData,humiData,smokeData,coData);
        }

    }   
}



void usartREC_task(void *pvParameters)
{    
    while (1)
    {    
        if(xQueueReceive(xQueueSerial, receivedData,portMAX_DELAY)) // 等待接收消息
        {     
            OLED_ShowString(4,1,receivedData);
            if(strstr(receivedData,"LED_ON"))
            {
                LED_ON();               
            }
            else if(strstr(receivedData,"LED_OFF"))
            {
                LED_OFF();              
            }
            else if(strstr(receivedData,"BUZZER_OFF"))
            {
                Buzzer_OFF();            
            }
            else if(strstr(receivedData,"BUZZER_ON"))
            {                
                Buzzer_ON();                   
            }
            else if(strstr(receivedData,"FAN_OFF"))
            {                
                OLED_ShowString(2,1,"FAN_OFF");             
            }
            else if(strstr(receivedData,"FAN_LOW"))
            {               
                OLED_ShowString(2,1,"FAN_LOW");                
            }
            else if(strstr(receivedData,"FAN_MEDIUM"))
            {                
                OLED_ShowString(2,1,"FAN_MEDIUM");               
            }
            else if(strstr(receivedData,"FAN_HIGH"))
            {               
                OLED_ShowString(2,1,"FAN_HIGH");                
            }
            else
            {
                
            }
        } 
        vTaskDelay(20);        
    }    
}


int main(void)
{      
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组 4      
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
    xSemaphoreGive(xSemaphore);
    OLED_ShowString(1, 1, "temp:");
    OLED_ShowString(2, 1, "humi:");
    OLED_ShowString(3, 1, "MQ2:");
    OLED_ShowString(4, 1, "MQ7:");
	DHT11_Show();
    smokeData = MQ2_GetPPM();//将vel字符串类型转换为数字存储到数组
    sprintf(buffer1,"%.2lf",smokeData);
    OLED_ShowString(3, 5, buffer1);
    coData=MQ7_GetPPM();
    sprintf(buffer2,"%.2lf",coData);
    OLED_ShowString(4, 5, buffer2);
    LED_ON();		 
    FAN_Speed(FAN_OFF);  
    //ESP8266_Init();
   
}

