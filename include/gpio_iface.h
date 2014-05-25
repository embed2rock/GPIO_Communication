/*
 * gpio_iface.h
 *
 *  Created on: 24/mag/2014
 *      Author: raidenfox
 */

#ifndef GPIO_IFACE_H_
#define GPIO_IFACE_H_

#include "stm32f4xx_gpio.h"
#include <stm32f4xx_usart.h>

// Parametri della trasmissione
#define MAX_STRLEN 12
volatile char received_string[MAX_STRLEN+1];

// Parametri della porta seriale
#define BAUDRATE 9600
#define SPEED 100000L

// Parametri di connessione
#define RCC_APB_DEVICE RCC_APB2Periph_USART1
#define ENABLED_GPIOS RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD
#define DEBUG_LEDS GPIO_Pin_12 | GPIO_Pin_13

#define TX_PIN_SOURCE GPIO_PinSource6
#define RX_PIN_SOURCE GPIO_PinSource7

#define TX_PIN GPIO_Pin_6
#define RX_PIN GPIO_Pin_7

void init_iface();
void setup_usart();
void USART_puts(USART_TypeDef*, volatile char *);

void Delay(__IO uint32_t nCount);


#endif /* GPIO_IFACE_H_ */
