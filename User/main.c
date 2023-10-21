#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "OLED.h"
#include "DHT.h"
#include "MQ2.h"

extern uint16_t AD_Value[4];
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

/*
//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void led1_task(void *pvParameters);
*/

 
 void start_task(void *pvParameters)
{
     taskENTER_CRITICAL();           //进入临界区
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,         
                (const char*    )"led0_task",       
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,  
                (UBaseType_t    )LED0_TASK_PRIO,    
                (TaskHandle_t*  )&LED0Task_Handler); 
    
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
//LED0 任务函数

DHT11_Data_TypeDef DHT11_Data;
static void DHT11_Show(void)
{
    if(Read_DHT11(&DHT11_Data) == SUCCESS)
    {
        OLED_ShowNum(2, 10, DHT11_Data.humi_int, 2);
        OLED_ShowString(2, 12, ".");
        OLED_ShowNum(2, 13, DHT11_Data.humi_deci, 2);
        
        OLED_ShowNum(1, 6, DHT11_Data.temp_int, 2);
        OLED_ShowString(1, 8, ".");
        OLED_ShowNum(1, 9, DHT11_Data.temp_deci, 1);      
    }else
	{
		OLED_ShowString(4,1,"hello");
	}
}
void led0_task(void *pvParameters)
{
     while(1)
     {
		 OLED_ShowString(1, 1, "temp:");
		 OLED_ShowString(2, 1, "humi:");
		 OLED_ShowString(3, 1, "MQ2:");
		 OLED_ShowNum(3, 5,MQ2_GetPPM(),4);
		 DHT11_Show();
     }
}

int main(void)
{  
     
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组 4 
     
    /*init*/
    OLED_Init();
	DHT11_GPIO_Config();
	MQ2_Init();
	TIM3_Init();
     //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

