#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "OLED.h"
#include "DHT.h"
#include "MQ2.h"
#include "Delay.h"
#include "string.h"
#include "Buzzer.h"
#include "USART.h"
#include "queue.h"
#include "FAN.h"
#include "ESP8266.h"
#include "event_groups.h"
#include "MQ7.h"
extern uint16_t AD_Value[4];
char buffer1[2];

DHT11_Data_TypeDef DHT11_Data;//��ʪ�ȴ���������
double smokeData;
uint8_t tempData;
uint8_t humiData;
double coData;
    
int num=0;


/*�������ı�־λ*/
#define TEMP_EVENT (1 << 0)
#define HUMI_EVENT (1 << 1)
#define SMOKE_EVENT (1 << 2)
#define CO_EVENT (1 << 3)


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO		5
//�����ջ��С	
#define LED0_STK_SIZE 		50  
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);

//�������ȼ�
#define TEMP_TASK_PRIO		3
//�����ջ��С	
#define TEMP_STK_SIZE 		50  
//������
TaskHandle_t TEMPTask_Handler;
//������
void temp_task(void *pvParameters);

//�������ȼ�
#define HUMI_TASK_PRIO		3
//�����ջ��С	
#define HUMI_STK_SIZE 		50  
//������
TaskHandle_t HUMITask_Handler;
//������
void humi_task(void *pvParameters);

//�������ȼ�
#define SMOKE_TASK_PRIO		3
//�����ջ��С	
#define SMOKE_STK_SIZE 		50  
//������
TaskHandle_t SMOKETask_Handler;
//������
void smoke_task(void *pvParameters);

//�������ȼ�
#define CO_TASK_PRIO		3
//�����ջ��С	
#define CO_STK_SIZE 		50  
//������
TaskHandle_t COTask_Handler;
//������
void co_task(void *pvParameters);


//�������ȼ�
#define USARTREC_TASK_PRIO		7
//�����ջ��С	
#define USARTREC_STK_SIZE 		50  
//������
TaskHandle_t USARTRECTask_Handler;
//������
void usartREC_task(void *pvParameters);



/*���ڽ������ݵ���Ϣ����*/
QueueHandle_t xQueueSerial;
/*���ڷ������ݵ��¼�*/
EventGroupHandle_t xEventGroupSensorData;

static void USART_test(void);
 
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
   
    xQueueSerial = xQueueCreate(5, sizeof(char[MAX_RX_DATA_LENGTH]));
    xEventGroupSensorData=xEventGroupCreate();
    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,         
                (const char*    )"led0_task",       
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,  
                (UBaseType_t    )LED0_TASK_PRIO,    
                (TaskHandle_t*  )&LED0Task_Handler); 
    
    //ʪ������
    xTaskCreate((TaskFunction_t )humi_task,         
                (const char*    )"humi_task",       
                (uint16_t       )HUMI_STK_SIZE, 
                (void*          )NULL,  
                (UBaseType_t    )HUMI_TASK_PRIO,    
                (TaskHandle_t*  )&HUMITask_Handler); 

    //�¶�����
    xTaskCreate((TaskFunction_t )temp_task,         
                (const char*    )"temp_task",       
                (uint16_t       )TEMP_STK_SIZE, 
                (void*          )NULL,  
                (UBaseType_t    )TEMP_TASK_PRIO,    
                (TaskHandle_t*  )&TEMPTask_Handler); 

    //��������
    xTaskCreate((TaskFunction_t )smoke_task,         
                (const char*    )"smoke_task",       
                (uint16_t       )SMOKE_STK_SIZE, 
                (void*          )NULL,  
                (UBaseType_t    )SMOKE_TASK_PRIO,    
                (TaskHandle_t*  )&SMOKETask_Handler); 


    //һ����̼����
    xTaskCreate((TaskFunction_t )co_task,         
                (const char*    )"co_task",       
                (uint16_t       )CO_STK_SIZE, 
                (void*          )NULL,  
                (UBaseType_t    )CO_TASK_PRIO,    
                (TaskHandle_t*  )&COTask_Handler);  
                
     //�������ڽ�������
    xTaskCreate((TaskFunction_t )usartREC_task,         
                (const char*    )"uasrtREC_task",       
                (uint16_t       )USARTREC_STK_SIZE , 
                (void*          )NULL,  
                (UBaseType_t    )USARTREC_TASK_PRIO,    
                (TaskHandle_t*  )&USARTRECTask_Handler); 
                
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
//LED0 ������


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
        /*
		 OLED_ShowString(1, 1, "temp:");
		 OLED_ShowString(2, 1, "humi:");
		 OLED_ShowString(3, 1, "MQ2:");
		 smokeData = MQ2_GetPPM();//��vel�ַ�������ת��Ϊ���ִ洢������
		 sprintf(buffer1,"%.2lf",smokeData);
		 OLED_ShowString(3, 5, buffer1);
		 DHT11_Show();
		 Buzzer_OFF();
		 FAN_Speed(FAN_OFF); 
        */         
         USART_test();
         vTaskDelay(2500);   
     }
}
void temp_task(void *pvParameters)
{
    while(1)
    {
        /*
        if(Read_DHT11(&DHT11_Data) == SUCCESS)
        {
            //xEventGroupSetBits(xEventGroupSensorData,
        */
    }
    
}

void humi_task(void *pvParameters)
{
    while(1)
    {
        
    }
    
}

void smoke_task(void *pvParameters)
{
    while(1)
    {
        
    }
    
}

void co_task(void *pvParameters)
{
    while(1)
    {
        
    }
    
}

void usartREC_task(void *pvParameters)
{
    char receivedData[MAX_RX_DATA_LENGTH];
    
    while (1)
    {                                                                                                                        
        if(xQueueReceive(xQueueSerial, receivedData,portMAX_DELAY)) // �ȴ�������Ϣ
        {            
            OLED_ShowString(4,1,receivedData);
            if(strstr(receivedData,"LED_ON"))
            {
                
                OLED_ShowString(2,1,"LED_ON");
                
            }
            else if(strstr(receivedData,"LED_OFF"))
            {
                
                OLED_ShowString(2,1,"LED_OFF");
                
            }
            else if(strstr(receivedData,"BUZZER_OFF"))
            {
                
                OLED_ShowString(2,1,"BUZZER_OFF");
                
            }
            else if(strstr(receivedData,"BUZZER_ON"))
            {
                
                OLED_ShowString(2,1,"BUZZER_ON");
                
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
    }    
}

static void USART_test(void)
{
     num++;
     OLED_ShowNum(1,1,num,4);
     printf("hello\r\n");
     //ESP8266_SendData(40,20,90.2,39.4);
}
int main(void)
{  
     
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ����� 4      
    /*init*/
    OLED_Init();
    USART1_Init();
	DHT11_GPIO_Config();
	MQ2_Init();
    MQ7_Init();
	TIM3_Init();
	Buzzer_Init();
	FAN_Init();
    //ESP8266_Init();
    
     //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

