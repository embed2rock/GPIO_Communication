/*
 * main.c
 *
 *  Created on: 24/mag/2014
 *      Author: raidenfox
 */

#include <stm32f4xx.h>
#include "gpio_iface.h"


int main(void) {

  init_iface();
  USART_puts(USART1, "Init complete! Hello World!rn"); // just send a message to indicate that it works

  while (1){
  }
}

