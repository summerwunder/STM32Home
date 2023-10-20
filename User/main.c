#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

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

//�������ȼ�
#define LED1_TASK_PRIO		3
//�����ջ��С	
#define LED1_STK_SIZE 		50  
//������
TaskHandle_t LED1Task_Handler;
//������
void led1_task(void *pvParameters);


 
void GPIO_INIT()
{
     
      GPIO_InitTypeDef  GPIO_InitStructure;
    
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��PB,PE�˿�ʱ��
            
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   //LED0-->PB.5 �˿�����
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO���ٶ�Ϊ50MHz
      GPIO_Init(GPIOA, &GPIO_InitStructure);  //�����趨������ʼ��GPIOB.5
      GPIO_SetBits(GPIOA,GPIO_Pin_0);     //PB.5 �����

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;   //LED1-->PE.5 �˿�����, �������
      GPIO_Init(GPIOA, &GPIO_InitStructure);  //������� ��IO���ٶ�Ϊ50MHz
      GPIO_SetBits(GPIOA,GPIO_Pin_5);     //PE.5 ����� 
}
 
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
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);         
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
//LED0 ������
void led0_task(void *pvParameters)
{
     while(1)
     {
          GPIO_SetBits(GPIOA,GPIO_Pin_5);
          vTaskDelay(500);
          GPIO_ResetBits(GPIOA,GPIO_Pin_5);
          vTaskDelay(500);
     }
}
//LED1 ������
void led1_task(void *pvParameters)
{
    while(1)
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_0);
        vTaskDelay(300);
        GPIO_SetBits(GPIOA,GPIO_Pin_0);
        vTaskDelay(300);
    }
}
int main(void)
{  
     
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ����� 4 
     GPIO_INIT();
     
     //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

