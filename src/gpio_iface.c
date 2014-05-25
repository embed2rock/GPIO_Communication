/*
 * gpio_iface.c
 *
 *  Created on: 24/mag/2014
 *      Author: raidenfox
 */

#include "gpio_iface.h"

void Delay(__IO uint32_t nCount) {
  while(nCount--) {
  }
}

void init_iface(){
	// Abilita il clock alla USART
	GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
	RCC_APB2PeriphClockCmd(RCC_APB_DEVICE, ENABLE);

	// Abilitazione del GPIO porto B e D
	RCC_AHB1PeriphClockCmd(ENABLED_GPIOS, ENABLE);


	// Configurazione GPIO di default per il debugging e la segnalazione delle trasmissioni
	GPIO_InitStruct.GPIO_Pin = DEBUG_LEDS;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	// Viene definita una configurazione per RX e TX settanto i relativi parametri elettrici
	GPIO_InitStruct.GPIO_Pin = RX_PIN | TX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*
	 * Sono assegnati i pin di RX e TX
	 */
	GPIO_PinAFConfig(GPIOB, RX_PIN_SOURCE, GPIO_AF_USART1); //
	GPIO_PinAFConfig(GPIOB, TX_PIN_SOURCE, GPIO_AF_USART1);


	// Connettiamo la USART ai pin di trasmssione e ricezione

	setup_usart();
}

void setup_usart(){
	// Struct per l'interrupt manager e per la USART
	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_InitTypeDef USART_InitStruct;

	/* Si settano le proprietà della trasmissione
	 * - parità
	 * - stop bit
	 * - controllo di flusso
	 * - modalità di trasmissione
	*/
	USART_InitStruct.USART_BaudRate = BAUDRATE;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStruct);

	// Abilita le interruzioni sul porto RX
	/* Si settano:
	 * - la linea di interruzione dedicata
	 * - la priorità
	 * - viene abilitata la linea
	 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_Cmd(USART1, ENABLE);
}


// Interrupt handler per l'arrivo di una trasmissione in ricezione/trasmissione
void USART1_IRQHandler(void){

	// Verifica se il registro in ricezione non è vuoto, se non lo è ha ricevuto un carattere
	if( USART_GetITStatus(USART1, USART_IT_RXNE) ){
		// Accende il LED 4
		GPIO_SetBits(GPIOD,GPIO_Pin_12);
		static uint8_t cnt = 0; // conta la lunghezza della stringa
		char t = USART1->DR; // preleva il carattere i-esimo

		// Verifica del carattere ricevuto, in particolare se la trasmissione è finita
		if( (t != 'n') && (cnt < MAX_STRLEN) ){
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
			Delay(500000L);
			GPIO_ResetBits(GPIOD,GPIO_Pin_13);
			Delay(500000L);
			received_string[cnt] = t;
			cnt++;
		}
		else{ // Se la trasmissione è finita si resetta il contatore
			cnt = 0;
			USART_puts(USART1, received_string);
		}
	}
}

void USART_puts(USART_TypeDef* USARTx, volatile char *s){

	while(*s){
		// Scorre lo shift register in trasmissione fino a che esso no è vuoto e invia l'esimo carattere
		while( !(USARTx->SR & 0x00000040) );
		USART_SendData(USARTx, *s);
		*s++;
	}
}
