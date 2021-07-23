/**
	* Weather API test program.
	*
	* ESP_Weather.h
	*
  * MCU: STM32F103C8T6
	* IDE: Keil uVision V5.29.0.0 & STM32CubeMX 5.6.0
	* HCLK : 40 MHz
	*
  * Wifi Module : ESP8266-01
	* 		AT version: 1.3.0.0
	* 		SDK version: 2.0.0
	* 
  * Copyright 2020 Gachon University, TAKE OUT, Development Group, electric engineering, Seongwook Kang.
  */
	
#ifndef ESP_WEATHER_H
#define ESP_WEATHER_H

#include "usart.h"
#include "gpio.h"

/* UART_Handler */
extern UART_HandleTypeDef huart1;

/* FLAGs */
/* Detects if all data is received. */
#define Rx_COMPLETE_READY		0
#define Rx_COMPLETE_WAIT		1

/* Detects if ESP initialize is completed. */
#define ESP_INITIALIZE_WAIT		0
#define ESP_INITIALIZE_DONE		1

/* Detects system timeout. */
#define SYSTEM_OK					0
#define SYSTEM_TIMEOUT		1


/* Detects if AT+RST is completed. */
#define ESP_RST_FAIL		0
#define ESP_RST_DONE		1

/* Detects if Echo turn off or on. */
#define ESP_ATE_ON		0
#define ESP_ATE_OFF		1

/* Detects if AT+CWMODE is completed. */
#define ESP_CWMODE_WAIT		0
#define ESP_CWMODE_DONE		1

/* Detects if AT+CWJAP is completed. */
#define ESP_CWJAP_WAIT		0
#define ESP_CWJAP_DONE		1

/* Detects if AT+CIFSR is completed. */
#define ESP_CIFSR_WAIT		0
#define ESP_CIFSR_DONE		1

/* Detects if AT+CIPSTART is completed. */
#define ESP_CIPSTART_WAIT		0
#define ESP_CIPSTART_DONE		1

/* Detects if AT+CIPSEND is completed. */
#define ESP_CIPSEND_WAIT		0
#define ESP_CIPSEND_DONE		1

/* Detects if AT+CIPSEND is completed. */
#define ESP_SENDHTTP_WAIT		0
#define ESP_SENDHTTP_DONE		1

/* Detects if AT+CIPCLOSE is completed. */
#define ESP_CIPCLOSE_WAIT		0
#define ESP_CIPCLOSE_DONE		1

/* rx_buffer */
uint8_t rx_Buffer = 0;
uint8_t rx_Data[450] = { 0, };

/* Command List for ESP-01 */
char* cmd;
char* checkSystem = "AT\r\n";
char* checkVersion = "AT+GMR\r\n";
char* turnoffEcho = "ATE0\r\n";
char* turnonEcho = "ATE1\r\n";
char* resetSystem = "AT+RST\r\n";
char* setWifimode = "AT+CWMODE=1\r\n";
char* connectWifi = "AT+CWJAP=\"Radii_S20\",\"11112345\"\r\n";
char* getIP = "AT+CIFSR\r\n";
char* startTCPConnection = "AT+CIPSTART=\"TCP\",\"api.openweathermap.org\",80\r\n";
char* sendCmd = "AT+CIPSEND=138\r\n";
char* sendHTTP = "GET /data/2.5/weather?APPID=9eb99614296bd4ca54f319f748a35119&id=1897000 HTTP/1.1\r\nHost: api.openweathermap.org\r\nConnection: keep-alive\r\n\r\n";
char* closeTCPConnection = "AT+CIPCLOSE\r\n";

/* AT Response List to ESP-01 */
uint8_t isComplete_length = 0;
char isComplete[15] = { 0, };
char* isComplete_checkSystem = "OK\r\n";
char* isComplete_turnoffEcho = "OK\r\n";
char* isComplete_turnonEcho = "OK\r\n";
char* isComplete_resetSystem = "WIFI GOT IP\r\n";
char* isComplete_setWifimode = "OK\r\n";
char* isComplete_connectWifi = "WIFI GOT IP\r\n";
char* isComplete_getIP = "OK\r\n";
char* isComplete_startTCPConnection = "OK\r\n";
char* isComplete_sendCmd = ">";
char* isComplete_sendHTTP = "\"cod\":200}";
char* isComplete_closeTCPConnection = "OK\r\n";

/* critcal flag */
struct critical_Flag {
	uint8_t Rx_Complete_Flag;
	uint8_t ESP_Init_Flag;
	uint8_t system_Timeout;
	
	uint8_t ESP_Rst_Flag;
	uint8_t ESP_Ate_Flag;
	uint8_t ESP_Cwmode_Flag;
	uint8_t ESP_Cwjap_Flag;
	uint8_t ESP_Cifsr_Flag;
	uint8_t ESP_Cipstart_Flag;
	uint8_t ESP_Cipsend_Flag;
	uint8_t ESP_Sendhttp_Flag;
	uint8_t ESP_Cipclose_Flag;
};
struct critical_Flag flags = { 0, };

/* index for data buffer or receive buffer */
uint8_t data_index = 0;
uint8_t receive_index = 0;
uint8_t init_process = 0;
uint8_t connect_process = 0;
uint8_t error_index = 0;
uint8_t closed_index = 0;

/* time var for timeout error */
uint32_t tickStart = 0U;
uint32_t systemTimeout = 10000; // ms

/* Updating Period */
uint32_t period = 1000U;
	

/* Function Prototypes */
void AT_Command(char* cmd);
void ESP_init(void);
void getTCPData(void);
char* matchingCmd(char* cmd);
uint8_t matchingFlag(char* cmd);
void getWeather(void);


#endif
