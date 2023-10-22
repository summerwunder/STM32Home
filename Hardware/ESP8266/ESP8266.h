#ifndef __ESP8266_H
#define __ESP8266_H


#include "USART.h"
#include "Delay.h"
#include "FreeRTOS.h"
#define ESP8266_WIFI_INFO "AT+CWJAP=\"nonetheless\",\"88888888\"\r\n"
#define ESP8266_INTERNET_INFO "AT+MQTTCONN=0,\"192.168.43.222\",1883,1\r\n"
#define ESP8266_MQTT_INFO "AT+MQTTUSERCFG=0,1,\"espHome\",\"esp8266\",\"123\",0,0,\"\"\r\n"
#define ESP8266_SUB_INFO "AT+MQTTSUB=0,\"control/#\",1\r\n"   


void ESP8266_Init(void);
void ESP8266_SendData(uint8_t humi,uint8_t temp,double smoke,double coDense);
#endif
