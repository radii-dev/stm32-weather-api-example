/**
	* Weather API test program.
	*
	* ESP_Weather.c
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
	
#include "ESP_Weather.h"
#include <string.h>

void getWeather(void) {
	if (flags.system_Timeout == SYSTEM_TIMEOUT) {
			rx_Buffer = 0;
			tickStart = 0U;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		if (flags.ESP_Init_Flag == ESP_INITIALIZE_DONE) {
			connect_process = 0;
			flags.ESP_Cipstart_Flag = ESP_CIPSTART_WAIT;
			flags.ESP_Sendhttp_Flag = ESP_SENDHTTP_WAIT;
			flags.ESP_Cipsend_Flag = ESP_CIPSEND_WAIT;
			flags.Rx_Complete_Flag = Rx_COMPLETE_READY;
			flags.system_Timeout = SYSTEM_OK;
		}
		else {
			init_process = 0;
			connect_process = 0;
			memset(&flags, 0, sizeof(struct critical_Flag));
		}
	}
	else {
		if (flags.ESP_Init_Flag == ESP_INITIALIZE_DONE) 
			getTCPData();
		else 
			ESP_init();
	}	
}

void AT_Command(char* cmdname) { 
	if (flags.Rx_Complete_Flag == Rx_COMPLETE_READY && matchingFlag(cmdname) == 0) {
		tickStart = HAL_GetTick();
		flags.Rx_Complete_Flag = Rx_COMPLETE_WAIT;
		data_index = 0;
		isComplete_length = 0;
		for(int i = 0; rx_Data[i]; i++) 
			rx_Data[i] = 0;
		for(int i = 0; isComplete[i]; i++) 
			isComplete[i] = 0;
		cmd = matchingCmd(cmdname);
		for(int i = 0; isComplete[i]; i++) 
			isComplete_length++;
		uint8_t cnt = 0;
		for(int i = 0; cmd[i]; i++)
			cnt++;
		HAL_UART_Transmit(&huart1, (uint8_t *)cmd, cnt, 15);
	}
	else {
		if ((HAL_GetTick() - tickStart) > systemTimeout) {
			flags.system_Timeout = SYSTEM_TIMEOUT;
		}
	}
}

uint8_t matchingFlag(char* cmd) {
	if (strcmp(cmd, "resetSystem\0") == 0) 
		return flags.ESP_Rst_Flag;
	else if (strcmp(cmd, "setWifimode\0") == 0) 
		return flags.ESP_Cwmode_Flag;
	else if (strcmp(cmd, "connectWifi\0") == 0) 
		return flags.ESP_Cwjap_Flag;
	else if (strcmp(cmd, "getIP\0") == 0) 
		return flags.ESP_Cifsr_Flag;
	else if (strcmp(cmd, "startTCPConnection\0") == 0) 
		return flags.ESP_Cipstart_Flag;
	else if (strcmp(cmd, "sendCmd\0") == 0) 
		return flags.ESP_Cipsend_Flag;
	else if (strcmp(cmd, "sendHTTP\0") == 0) 
		return flags.ESP_Sendhttp_Flag;
	else if (strcmp(cmd, "closeTCPConnection\0") == 0) 
		return flags.ESP_Cipclose_Flag;
}

char* matchingCmd(char* cmd) {
	if (strcmp(cmd, "resetSystem\0") == 0) {
		strcpy(isComplete, isComplete_resetSystem);
		return resetSystem;
	}
	else if (strcmp(cmd, "setWifimode\0") == 0) {
		strcpy(isComplete, isComplete_setWifimode);
		return setWifimode;
	}
	else if (strcmp(cmd, "connectWifi\0") == 0) {
		strcpy(isComplete, isComplete_connectWifi);
		return connectWifi;
	}
	else if (strcmp(cmd, "getIP\0") == 0) {
		strcpy(isComplete, isComplete_getIP);
		return getIP;
	}
	else if (strcmp(cmd, "startTCPConnection\0") == 0) {
		strcpy(isComplete, isComplete_startTCPConnection);
		return startTCPConnection;
	}
	else if (strcmp(cmd, "sendCmd\0") == 0) {
		strcpy(isComplete, isComplete_sendCmd);
		return sendCmd;
	}
	else if (strcmp(cmd, "sendHTTP\0") == 0) {
		strcpy(isComplete, isComplete_sendHTTP);
		return sendHTTP;
	}
	else if (strcmp(cmd, "closeTCPConnection\0") == 0) {
		strcpy(isComplete, isComplete_closeTCPConnection);
		return closeTCPConnection;
	}
	else {
		flags.system_Timeout = SYSTEM_TIMEOUT;
		return 0;
	}
}

void ESP_init(void) {
	switch (init_process) {
		case 0:
			if (flags.ESP_Rst_Flag == ESP_RST_DONE)
				init_process = 3;
			else 
				AT_Command("resetSystem");
			break;
			/*
		case 1:
			if (flags.ESP_Cwmode_Flag == ESP_CWMODE_DONE)
				init_process = 2;
			else
				AT_Command("setWifimode");
			break;
		case 2:
			if (flags.ESP_Cwjap_Flag == ESP_CWJAP_DONE)
				init_process = 3;
			else 
				AT_Command("connectWifi");
			break;
			*/
		case 3:
			if (flags.ESP_Cifsr_Flag == ESP_CIFSR_DONE) {
				flags.ESP_Init_Flag = ESP_INITIALIZE_DONE;
				init_process = 4;
			}
			else{
				flags.ESP_Cwmode_Flag = ESP_CWMODE_DONE;
				flags.ESP_Cwjap_Flag = ESP_CWJAP_DONE;
				AT_Command("getIP");
			}
			break;
	}
}

void getTCPData(void) {
	switch(connect_process) {
		case 0:
			if (flags.ESP_Cipstart_Flag == ESP_CIPSTART_DONE)
				connect_process = 1;
			else 
				AT_Command("startTCPConnection");
			break;
		case 1:
			if (flags.ESP_Cipsend_Flag == ESP_CIPSEND_DONE) {
				flags.ESP_Sendhttp_Flag = ESP_SENDHTTP_WAIT;
				connect_process = 2;
			}
			else 
				AT_Command("sendCmd");
			break;
		case 2:
			if (flags.ESP_Sendhttp_Flag == ESP_SENDHTTP_DONE){
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
				flags.ESP_Cipsend_Flag = ESP_CIPSEND_WAIT;
				connect_process = 1;
			}
			else {
				AT_Command("sendHTTP");
				HAL_Delay(period);
			}
			break;
			/*
		case 3:
			if (flags.ESP_Cipclose_Flag == ESP_CIPCLOSE_DONE) 
				connect_process = 0;
			else 
				AT_Command("closeTCPConnection");
			break;
			*/
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) { 
		HAL_UART_Receive_IT(&huart1, &rx_Buffer, 1);
		rx_Data[data_index++] = rx_Buffer;
		if (rx_Buffer == isComplete[receive_index]) 
			receive_index++;
		else 
			receive_index = 0;
		
		if (receive_index >= isComplete_length) {
			if (strcmp(isComplete, "OK\r\n") == 0) {
				if (strcmp(cmd, setWifimode) == 0) 
					flags.ESP_Cwmode_Flag = ESP_CWMODE_DONE;
				else if (strcmp(cmd, getIP) == 0) 
					flags.ESP_Cifsr_Flag = ESP_CIFSR_DONE;
				else if (strcmp(cmd, startTCPConnection) == 0) 
					flags.ESP_Cipstart_Flag = ESP_CIPSTART_DONE;
				else if (strcmp(cmd, closeTCPConnection) == 0) 
					flags.ESP_Cipclose_Flag = ESP_CIPCLOSE_DONE;
			}
			else if (strcmp(isComplete, "WIFI GOT IP\r\n") == 0) {
				if (strcmp(cmd, resetSystem) == 0) 
					flags.ESP_Rst_Flag = ESP_RST_DONE;
				else if (strcmp(cmd, connectWifi) == 0) 
					flags.ESP_Cwjap_Flag = ESP_CWJAP_DONE;
			}
			else if (strcmp(isComplete, isComplete_sendCmd) == 0) {
				if (strcmp(cmd, sendCmd) == 0)
					flags.ESP_Cipsend_Flag = ESP_CIPSEND_DONE;
			}
			else if (strcmp(isComplete, isComplete_sendHTTP) == 0) {
				if (strcmp(cmd, sendHTTP) == 0)
					flags.ESP_Sendhttp_Flag = ESP_SENDHTTP_DONE;
			}
			else if (strcmp(isComplete, isComplete_sendHTTP) == 0)
			receive_index = 0;
			flags.Rx_Complete_Flag = Rx_COMPLETE_READY;
		}
	}
}
