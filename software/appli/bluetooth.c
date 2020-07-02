/*
 * bluetooth.c

 *
 *  Created on: 22 mars 2020
 *      Author: martin
 */


#include "bluetooth.h"


/* !! Ne pas oublier d'initialiser les UART dans le main :
		UART_init(UART6_ID,9600);	//bluetooth  //COM20 sur mon pc
		UART_init(UART2_ID,9600);  //printf		//COM17 sur mon pc
		//on utlise l'UART 2 pour printf(..)
		SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);
*/


		//fonction
		//envoie donnée PC sous forme d'une trame de 4 informations
		//Rajouter la fonction UART_puts(..,..,..) dans la librairie de l'UART + definir des variables

		void envoiTrame(uint8_t info1, uint8_t info2, uint8_t info3, uint8_t info4){
			static uint8_t trame[4];
			trame[0] = info1;
			trame[1] = info2;
			trame[2] = info3;
			trame[3] = info4;
			UART_puts(UART2_ID, trame, 4);
		}

		//envoie un caractère
		void envoiCaractere(uint8_t c){
			c=UART_get_next_byte(UART2_ID);
			UART_putc(UART2_ID,c);
		}

		//reception d'un caractère
		void receptionCaractere(){
			if(UART_data_ready(UART2_ID)){
				uint8_t c;
				c = UART_get_next_byte(UART2_ID);
				//printf("%c",c);
			}
		}
		//reception d'une Trame de 4 informations
		void receptionTrame(){
			if(UART_data_ready(UART2_ID)){
				uint8_t trame[4];
				UART_gets(UART1_ID, trame, 7);
				uint8_t info1 = *trame;
				uint8_t info2 =*(trame+1);
				uint8_t info3 = *(trame+2);
				uint8_t info4 =*(trame+3);
				printf("%d %d %d %d",info1,info2,info3,info4);
			}
		}


