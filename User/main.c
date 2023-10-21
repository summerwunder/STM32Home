#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "OLED.h"
#include "DHT.h"
#include "MQ2.h"

extern uint16_t AD_Value[4];
//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO		2
//�����ջ��С	
#define LED0_STK_SIZE 		50  
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);

/*
//�������ȼ�
#define LED1_TASK_PRIO		3
//�����ջ��С	
#define LED1_STK_SIZE 		50  
//������
TaskHandle_t LED1Task_Handler;
//������
void led1_task(void *pvParameters);
*/

 
 void start_task(void *pvParameters)
{
     taskENTER_CRITICAL();           //�����ٽ���
    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,         
                (const char*    )"led0_task",       
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,  
                (UBaseType_t    )LED0_TASK_PRIO,    
                (TaskHandle_t*  )&LED0Task_Handler); 
    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
//LED0 ������

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
     
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ����� 4 
     
    /*init*/
    OLED_Init();
	DHT11_GPIO_Config();
	MQ2_Init();
	TIM3_Init();
     //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

