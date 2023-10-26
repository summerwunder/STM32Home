#include "stm32f10x.h"                  // Device header
#include "ESP8266.h"

void ESP8266_Init(void)
{
    printf("AT\r\n");//第一步		AT
    Delay_ms(1000);	//延迟  

    printf("AT+RST\r\n");
    Delay_ms(1000);
    
    printf("AT+CWMODE=1\r\n");
    Delay_ms(1000);
    
    printf("AT+CIPMUX=1\r\n");
    Delay_ms(1000);
    
    printf(ESP8266_WIFI_INFO);
    Delay_ms(4000);
        
    USART_SendString(ESP8266_MQTT_INFO);
    Delay_ms(2000);
    
    printf(ESP8266_INTERNET_INFO);
    Delay_ms(3000);
    
    printf(ESP8266_SUB_INFO);
    Delay_ms(1000);      
}

//AT+MQTTPUB=0,"esp/1","{\"temp\":31\,\"humi\":12\,\"coValue\":15.6\,\"smokeValue\":29.4}",1,0
void ESP8266_SendData(uint8_t humi,uint8_t temp,double smoke,double coDense)
{
    char pubtopic[100]="esp/1";
    char cmdBuf[512];  
    memset(cmdBuf,0,sizeof(cmdBuf));      
    sprintf(cmdBuf, "AT+MQTTPUB=0,\"%s\",\"{\\\"humi\\\":%d\\, \\\"temp\\\":%d\\, \\\"smokeValue\\\":%.1lf\\, \\\"coValue\\\":%.1lf}\",1,0\r\n",pubtopic,humi,temp,smoke,coDense);//发送命令
    printf("%s",cmdBuf);  
}
