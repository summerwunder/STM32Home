#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "OLED.h"
#include "DHT.h"
#include "MQ2.h"
#include "Delay.h"
#include "Buzzer.h"
#include "USART.h"
#include "queue.h"
#include "FAN.h"

extern uint16_t AD_Value[4];
char buffer1[2];
float mq2ppm;
int num=0;
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);


//任务优先级
#define USARTREC_TASK_PRIO		7
//任务堆栈大小	
#define USARTREC_STK_SIZE 		50  
//任务句柄
TaskHandle_t USARTRECTask_Handler;
//任务函数
void usartREC_task(void *pvParameters);



/*这是串口接收数据的消息队列*/


QueueHandle_t xQueueSerial;


static void USART_test(void);
 
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
   
    xQueueSerial = xQueueCreate(5, sizeof(char[MAX_RX_DATA_LENGTH]));
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,         
                (const char*    )"led0_task",       
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,  
                (UBaseType_t    )LED0_TASK_PRIO,    
                (TaskHandle_t*  )&LED0Task_Handler); 
    
                
     //创建串口接收任务
    xTaskCreate((TaskFunction_t )usartREC_task,         
                (const char*    )"uasrtREC_task",       
                (uint16_t       )USARTREC_STK_SIZE , 
                (void*          )NULL,  
                (UBaseType_t    )USARTREC_TASK_PRIO,    
                (TaskHandle_t*  )&USARTRECTask_Handler); 
                
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
//LED0 任务函数

DHT11_Data_TypeDef DHT11_Data;
static void DHT11_Show(void)
{
    if(Read_DHT11(&DHT11_Data) == SUCCESS)
    {
		
        OLED_ShowNum(2, 6, DHT11_Data.humi_int, 2);
        OLED_ShowNum(1, 6, DHT11_Data.temp_int, 2);
             
    }
}

void led0_task(void *pvParameters)
{
     while(1)
     {
        
		 OLED_ShowString(1, 1, "temp:");
		 OLED_ShowString(2, 1, "humi:");
		 OLED_ShowString(3, 1, "MQ2:");
		 mq2ppm = MQ2_GetPPM();//将vel字符串类型转换为数字存储到数组
		 sprintf(buffer1,"%.2f",mq2ppm);
		 OLED_ShowString(3, 5, buffer1);
		 DHT11_Show();
		 Buzzer_OFF();
		 FAN_Speed(FAN_OFF);
              
         //USART_test();
     }
}


void usartREC_task(void *pvParameters)
{
    char receivedData[MAX_RX_DATA_LENGTH];
    
    while (1)
    {
        if(xQueueReceive(xQueueSerial, receivedData, portMAX_DELAY)) // 等待接收消息
        {
            // receivedData 包含接收到的数据
            //printf("%s",receivedData);
            OLED_ShowString(2,1,receivedData);
        }
    }
    
       
}

static void USART_test(void)
{
     num++;
     OLED_ShowNum(1,1,num,4);
     printf("hello world\r\n");
     vTaskDelay(2500);
}
int main(void)
{  
     
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组 4      
    /*init*/
    OLED_Init();
	DHT11_GPIO_Config();
	MQ2_Init();
	TIM3_Init();
	Buzzer_Init();
	FAN_Init();
    //USART1_Init();
     //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

