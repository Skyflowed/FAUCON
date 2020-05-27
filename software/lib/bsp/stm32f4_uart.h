/*
 *	Club Robot ESEO 2008 - 2015
 *	Archi-Tech' / Pierre & Guy / Holly & Wood
 *
 *	Fichier : QS_uart.h
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des uart pour
 *				interfacage rs232
 *	Auteur : Jacen / Alexis / Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100924
 */

/** ----------------  Defines possibles  --------------------
 *	USE_UARTx					: Active l'utilisation de l'UART n°x [1..6]
 *
 *
 */

#ifndef QS_UART_H
	#define QS_UART_H
	#include "stm32f4xx_hal.h"
	#include "config.h"

	typedef void (*UART_dataReceiver)(Uint8 byte);

	typedef enum
	{
		UART1_ID = 0,
		UART2_ID,
		UART3_ID,
		UART4_ID,
		UART5_ID,
		UART6_ID,
		UART_ID_NB
	}UART_id_e;


	void UART_init(void);										//initialiser tout les uart définis
	void UART_deinit(void);
	void UART_init_id(UART_id_e id, uint32_t baudrate, bool_e flowControl, Uint8 irqPreemptionPriority);			//initialiser manuellement un uart spécifique
	void UART_setReceiveCallback(UART_id_e id, UART_dataReceiver function);
	void UART_setSendCallback(UART_id_e id, UART_dataReceiver function);
	void UART_set_std(UART_id_e in, UART_id_e out, UART_id_e err);	//modifier l'entrée où la sortie standard
	void UART_deinit_id(UART_id_e id);


	Uint32 UART_data_ready(UART_id_e id);							//récupérer le nombre d'octets disponibles
	Uint8 UART_get_next_msg(UART_id_e id);							//récupérer un octet (0 si pas d'octet dispo)
	void UART_putc(UART_id_e id, Uint8 c);							//envoyer octet
	void UART_puts(UART_id_e id, Uint8 * str);						//envoyer une chaine de caractères
	void UART_send_datas(UART_id_e id, Uint8 * datas, Uint32 len);	//envoyer un tableau de len octets

	char UART_read_stdin(void);
	void UART_write_stdout(char c);
	void UART_write_sterr(char c);

#endif /* ndef QS_UART_H */
