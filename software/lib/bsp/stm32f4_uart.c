/*
 *	Club Robot ESEO 2008 - 2015
 *	Archi-Tech', Pierre & Guy, Holly & Wood
 *
 *	Fichier : QS_uart.c
 *	Package : Qualite Soft
 *	Description : fonction d'utilisation des uart pour
 *					interfacage rs232
 *	Auteur : Jacen / Alexis / Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100924
 */

#include "stm32f4_uart.h"
#include "stm32f4xx_hal.h"
#include "config.h"
#include "stm32f4_gpio.h"
#include <stdio.h>

//*************************************************************************************************************************
//*************************************************      Define      ******************************************************
//*************************************************************************************************************************

#ifndef USE_UART1
	#define USE_UART1	0
#endif
#ifndef USE_UART2
	#define USE_UART2	0
#endif
#ifndef USE_UART3
	#define USE_UART3	0
#endif
#ifndef USE_UART4
	#define USE_UART4	0
#endif
#ifndef USE_UART5
	#define USE_UART5	0
#endif
#ifndef USE_UART6
	#define USE_UART6	0
#endif

#ifndef UART1_BAUDRATE
	#define UART1_BAUDRATE 115200
#endif
#ifndef UART2_BAUDRATE
	#define UART2_BAUDRATE 115200
#endif
#ifndef UART3_BAUDRATE
	#define UART3_BAUDRATE 115200
#endif
#ifndef UART4_BAUDRATE
	#define UART4_BAUDRATE 115200
#endif
#ifndef UART5_BAUDRATE
	#define UART5_BAUDRATE 115200
#endif
#ifndef UART6_BAUDRATE
	#define UART6_BAUDRATE 115200
#endif

#ifndef USART_1_IT_PRIORITY
	#define USART_1_IT_PRIORITY 3
#endif
#ifndef USART_2_IT_PRIORITY
	#define USART_2_IT_PRIORITY 3
#endif
#ifndef USART_3_IT_PRIORITY
	#define USART_3_IT_PRIORITY 3
#endif
#ifndef USART_4_IT_PRIORITY
	#define USART_4_IT_PRIORITY 3
#endif
#ifndef USART_5_IT_PRIORITY
	#define USART_5_IT_PRIORITY 3
#endif
#ifndef USART_6_IT_PRIORITY
	#define USART_6_IT_PRIORITY 3
#endif

#ifndef USART_1_FLOW_CONTROL
	#define USART_1_FLOW_CONTROL 0
#endif
#ifndef USART_2_FLOW_CONTROL
	#define USART_2_FLOW_CONTROL 0
#endif
#ifndef USART_3_FLOW_CONTROL
	#define USART_3_FLOW_CONTROL 0
#endif
#ifndef USART_4_FLOW_CONTROL
	#define USART_4_FLOW_CONTROL 0
#endif
#ifndef USART_5_FLOW_CONTROL
	#define USART_5_FLOW_CONTROL 0
#endif
#ifndef USART_6_FLOW_CONTROL
	#define USART_6_FLOW_CONTROL 0
#endif

//TODO reparcourir tout les USE_UART pour les définir à 1
//TODO : redéfinir les macros UART_FOR_PRINTF à USART1 au lieu de 1
#undef USART_FOR_PRINTF

#ifndef USART_FOR_PRINTF
	#define USART_FOR_PRINTF		USART1	//Default uart for output : 1.
#endif

#ifndef RX_BUFFER_SIZE
	#define RX_BUFFER_SIZE 1024
#endif

#ifndef TX_BUFFER_SIZE
	#define TX_BUFFER_SIZE 1024
#endif

#if (RX_BUFFER_SIZE & RX_BUFFER_SIZE-1)
	#warning "RX_BUFFER_SIZE doit être une puissance de 2, parce que c'est mieux ainsi."
#endif
#if (TX_BUFFER_SIZE & TX_BUFFER_SIZE-1)
	#warning "TX_BUFFER_SIZE doit être une puissance de 2, parce que c'est mieux ainsi."
#endif

#define USE_UART_NB (USE_UART1 + USE_UART2 + USE_UART3 + USE_UART4 + USE_UART5 + USE_UART6)

//*************************************************************************************************************************
//**************************************************      Type      *******************************************************
//*************************************************************************************************************************

typedef struct
{
	Uint8 buffer_rx[RX_BUFFER_SIZE];
	Uint8 buffer_tx[TX_BUFFER_SIZE];
	bool_e used;
}buffers_t;

typedef struct
{
	Uint32 rx_index_in;	//index des données qui entrent dans le buffer
	Uint32 rx_index_out;	//index des données qui sont sorties du buffer
	bool_e rx_max_occupation;
	Uint8 * prxbuf;
	Uint32 tx_index_in;
	Uint32 tx_index_out;
	bool_e tx_max_occupation;
	Uint8 * ptxbuf;
	UART_dataReceiver callback_receive;	//fonction de callback appelée à chaque caractère reçu
	UART_dataReceiver callback_send;	//fonction de callback appelée à chaque caractère émis
	Uint8 buffer_id;
	bool_e initialized;
	bool_e reentrance_detection_putc;
}UART_handler_t;

//*************************************************************************************************************************
//************************************************      Prototype      *****************************************************
//*************************************************************************************************************************

static bool_e UART_process_irq(UART_id_e id);
static void UART_ports_init(void);

//*************************************************************************************************************************
//********************************************      Variable globale      *************************************************
//*************************************************************************************************************************

static UART_HandleTypeDef UART_HandleStructure[UART_ID_NB];	//Ce tableau contient les structures qui sont utilisées pour piloter chaque UART avec la librairie HAL.
const USART_TypeDef * instance_array[UART_ID_NB] = {USART1, USART2, USART3, UART4, UART5, USART6};
const IRQn_Type nvic_irq_array[UART_ID_NB] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};


//const uint32_t rcc_uarts[UART_ID_NB] = { RCC_APB2Periph_USART1,RCC_APB1Periph_USART2, RCC_APB1Periph_USART3, RCC_APB1Periph_UART4, RCC_APB1Periph_UART5, RCC_APB2Periph_USART6};
const IRQn_Type irqn[UART_ID_NB] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};
USART_TypeDef * uarts[UART_ID_NB] = { USART1, USART2, USART3, UART4, UART5, USART6};

static volatile UART_handler_t handlers[UART_ID_NB];
static volatile bool_e initialized = FALSE;
static volatile UART_id_e uart_for_stdin_id;
static volatile UART_id_e uart_for_stdout_id;
static volatile UART_id_e uart_for_stderr_id;

static volatile buffers_t buffers[USE_UART_NB];

void UART_init(void)
{
	if(!initialized)
	{
		initialized = TRUE;
		uart_for_stdout_id = DEFAULT_PRINTF_UART;	//par défaut
		uart_for_stdin_id = DEFAULT_PRINTF_UART;
		uart_for_stderr_id = DEFAULT_PRINTF_UART;

		UART_ports_init();

		#if USE_UART1
			UART_init_id(UART1_ID, UART1_BAUDRATE, USART_1_FLOW_CONTROL, USART_1_IT_PRIORITY);
		#endif
		#if USE_UART2
			UART_init_id(UART2_ID, UART2_BAUDRATE, USART_2_FLOW_CONTROL, USART_2_IT_PRIORITY);
		#endif
		#if USE_UART3
			UART_init_id(UART3_ID, UART3_BAUDRATE, USART_3_FLOW_CONTROL, USART_3_IT_PRIORITY);
		#endif
		#if USE_UART4
			UART_init_id(UART4_ID, UART4_BAUDRATE, USART_4_FLOW_CONTROL, USART_4_IT_PRIORITY);
		#endif
		#if USE_UART5
			UART_init_id(UART5_ID, UART5_BAUDRATE, USART_5_FLOW_CONTROL, USART_5_IT_PRIORITY);
		#endif
		#if USE_UART6
			UART_init_id(UART6_ID, UART6_BAUDRATE, USART_6_FLOW_CONTROL, USART_6_IT_PRIORITY);
		#endif

	}
}

static void UART_ports_init(void)
{
#if USE_UART1
	__HAL_RCC_GPIOB_CLK_ENABLE();		//Horloge des broches a utiliser
	BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_6, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART1); //Configure Tx as AF
	BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_7, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART1); //Configure Rx as AF
	__HAL_RCC_USART1_CLK_ENABLE();		//Horloge du peripherique UART
#endif
#if USE_UART2
	__HAL_RCC_GPIOA_CLK_ENABLE();		//Horloge des broches a utiliser
	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_2, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2);	//Configure Tx as AF
	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_3, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2); //Configure Rx as AF
	__HAL_RCC_USART2_CLK_ENABLE();		//Horloge du peripherique UART
#endif
#if USE_UART3
	__HAL_RCC_GPIOD_CLK_ENABLE();		//Horloge des broches a utiliser
	BSP_GPIO_PinCfg(GPIOD, GPIO_PIN_8, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2);	//Configure Tx as AF
	BSP_GPIO_PinCfg(GPIOD, GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2); //Configure Rx as AF
	__HAL_RCC_USART3_CLK_ENABLE();		//Horloge du peripherique UART
#endif
#if USE_UART4
	__HAL_RCC_GPIOC_CLK_ENABLE();		//Horloge des broches a utiliser
	BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_10, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2);	//Configure Tx as AF
	BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_11, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2); //Configure Rx as AF
	__HAL_RCC_UART4_CLK_ENABLE();		//Horloge du peripherique UART
#endif
#if USE_UART5
	__HAL_RCC_GPIOC_CLK_ENABLE();		//Horloge des broches a utiliser
	__HAL_RCC_GPIOD_CLK_ENABLE();		//Horloge des broches a utiliser
	BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_12, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2);	//Configure Tx as AF
	BSP_GPIO_PinCfg(GPIOD, GPIO_PIN_2, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2); //Configure Rx as AF
	__HAL_RCC_UART5_CLK_ENABLE();		//Horloge du peripherique UART
#endif
#if USE_UART6
	__HAL_RCC_GPIOC_CLK_ENABLE();		//Horloge des broches a utiliser
	BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_6, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF8_USART6); //Configure Tx as AF
	BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_7, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF8_USART6); //Configure Rx as AF
	__HAL_RCC_USART6_CLK_ENABLE();		//Horloge du peripherique UART
#endif
}
void UART_deinit(void)
{
	UART_id_e id;
	for(id = 0; id<UART_ID_NB; id++)
		UART_deinit_id(id);
	initialized = FALSE;
}

void UART_init_id(UART_id_e id, uint32_t baudrate, bool_e flowControl, Uint8 irqPreemptionPriority)
{
	if(initialized && handlers[id].initialized == FALSE)
	{
		for(Uint8 i = 0; i<USE_UART_NB; i++)
		{	//recherche d'un couple de buffer disponibles
			if(buffers[i].used == FALSE)
			{
				handlers[id].rx_index_in = 0;
				handlers[id].rx_index_out = 0;
				handlers[id].rx_max_occupation = 0;
				handlers[id].tx_index_in = 0;
				handlers[id].tx_index_out = 0;
				handlers[id].tx_max_occupation = 0;
				handlers[id].callback_receive = NULL;
				handlers[id].callback_send = NULL;
				handlers[id].initialized = TRUE;

				//On associe les buffers à l'UART initialisé
				buffers[i].used = TRUE;
				handlers[id].buffer_id = i;	//on conserve l'index des buffers choisis
				handlers[id].prxbuf = (Uint8 *)buffers[i].buffer_rx;
				handlers[id].ptxbuf = (Uint8 *)buffers[i].buffer_tx;

				UART_HandleStructure[id].Instance = (USART_TypeDef*)instance_array[id];
				UART_HandleStructure[id].Init.BaudRate = baudrate;
				UART_HandleStructure[id].Init.WordLength = UART_WORDLENGTH_8B;//
				UART_HandleStructure[id].Init.StopBits = UART_STOPBITS_1;//
				UART_HandleStructure[id].Init.Parity = UART_PARITY_NONE;//
				UART_HandleStructure[id].Init.HwFlowCtl = UART_HWCONTROL_NONE;//
				UART_HandleStructure[id].Init.Mode = UART_MODE_TX_RX;//
				UART_HandleStructure[id].Init.OverSampling = UART_OVERSAMPLING_8;//

				/*On applique les parametres d'initialisation ci-dessus */
				HAL_UART_Init(&UART_HandleStructure[id]);

				/*Activation de l'UART */
				__HAL_UART_ENABLE(&UART_HandleStructure[id]);

				// On fixe les priorités des interruptions de usart6 PreemptionPriority = 0, SubPriority = 1 et on autorise les interruptions
				HAL_NVIC_SetPriority(nvic_irq_array[id] , irqPreemptionPriority, 1);
				HAL_NVIC_EnableIRQ(nvic_irq_array[id]);


/*
 * 				//on initialise le périphérique
				if(id == UART1_ID || id == UART6_ID)
					RCC_APB2PeriphClockCmd(rcc_uarts[id], ENABLE);
				else if(id < UART_ID_NB)
					RCC_APB1PeriphClockCmd(rcc_uarts[id], ENABLE);

				USART_OverSampling8Cmd((USART_TypeDef *)uarts[id], ENABLE);
				USART_InitTypeDef USART_InitStructure;
				USART_InitStructure.USART_BaudRate = baudrate;
				USART_InitStructure.USART_WordLength = USART_WordLength_8b;
				USART_InitStructure.USART_StopBits = USART_StopBits_1;
				USART_InitStructure.USART_Parity = USART_Parity_No;

				USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

				if(flowControl){
					USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
				}else{
					USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
				}

				USART_Init((USART_TypeDef *)uarts[id], &USART_InitStructure);

				NVIC_InitTypeDef NVIC_InitStructure;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
				NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = irqPreemptionPriority;
				NVIC_InitStructure.NVIC_IRQChannel = irqn[id];
				NVIC_Init(&NVIC_InitStructure);
				USART_ITConfig((USART_TypeDef *)uarts[id], USART_IT_RXNE, ENABLE);

				USART_Cmd((USART_TypeDef *)uarts[id], ENABLE);
*/



				break;
			}
		}
	}
}

//TODO modifier les appels à cette fonction
/*
 * @post Attention, cette fonction de callback sera appelée en IT uart, Assumez qu'elle soit courte et qu'il n'y aura pas de pb de préemption !
 */
void UART_setReceiveCallback(UART_id_e id, UART_dataReceiver function){
	assert(id > 0 && id < UART_ID_NB);
	handlers[id].callback_receive = function;
}

void UART_setSendCallback(UART_id_e id, UART_dataReceiver function){
	assert(id > 0 && id < UART_ID_NB);
	handlers[id].callback_send = function;
}

void UART_set_std(UART_id_e in, UART_id_e out, UART_id_e err)
{
	uart_for_stdout_id = out;
	uart_for_stdin_id = in;
	uart_for_stderr_id = err;
}

void UART_deinit_id(UART_id_e id){
/*
	if(id == UART1_ID || id == UART6_ID)
		RCC_APB2PeriphClockCmd(rcc_uarts[id], DISABLE);
	else if(id < UART_ID_NB)
		RCC_APB1PeriphClockCmd(rcc_uarts[id], DISABLE);
*/
	if(handlers[id].initialized)
	{
		//on dissocie les buffers
		buffers[handlers[id].buffer_id].used = FALSE;
		handlers[id].initialized = FALSE;
	}
}


Uint32 UART_data_ready(UART_id_e id)
{
	Uint32 ret = 0;
	Uint32 in, out;
	if(handlers[id].initialized)
	{
		NVIC_DisableIRQ(irqn[id]);
			//section critique ////////////////
			in = handlers[id].rx_index_in;
			//section critique ///////////////
		NVIC_EnableIRQ(irqn[id]);
		out = handlers[id].rx_index_out;
	}
		ret = out-in;
	return ret;
}

Uint8 UART_get_next_msg(UART_id_e id)
{
	Uint8 ret = 0;
	if(handlers[id].initialized)
	{
		uint32_t out, in;
		out = handlers[id].rx_index_out;
		NVIC_DisableIRQ(irqn[id]);
			//section critique ////////////////
			in = handlers[id].rx_index_in;
			//section critique ///////////////
		NVIC_EnableIRQ(irqn[id]);
		handlers[id].rx_max_occupation = MAX(handlers[id].rx_max_occupation, out-in);
		if(out < in)
		{
			ret = handlers[id].prxbuf[out % RX_BUFFER_SIZE];
			handlers[id].rx_index_out = out+1;
		}
		if(handlers[id].rx_max_occupation > RX_BUFFER_SIZE)
		{
			printf("WARNING : Buffer overflow on uart%d", id);
		}
	}
	return ret;
}

/*
 * @pre : JAMAIS de UART_putc dans une IT plus prioritaire que l'IT de l'UART...
 */
void UART_putc(UART_id_e id, Uint8 c)
{
	if(handlers[id].reentrance_detection_putc)	//Si printf en IT pendant un printf, on abandonne le caractère du printf en IT..
		return;
	handlers[id].reentrance_detection_putc = TRUE;

	Uint32 in, out;
	in = handlers[id].tx_index_in;
	out = handlers[id].tx_index_out;

	while(in-out >= TX_BUFFER_SIZE)
	{
		out = handlers[id].tx_index_out;
	}

	NVIC_DisableIRQ(irqn[id]);
	//section critique
		handlers[id].ptxbuf[in % RX_BUFFER_SIZE] = c;
		handlers[id].tx_index_in = in + 1;

		//USART_ITConfig((USART_TypeDef *)uarts[id], USART_IT_TXE, ENABLE);
		uarts[id]->CR1 |= USART_IT_TXE & USART_IT_MASK;

	//section critique
	NVIC_EnableIRQ(irqn[id]);

	if(handlers[id].callback_send)
		handlers[id].callback_send(c);

	handlers[id].reentrance_detection_putc = FALSE;
}

void UART_puts(UART_id_e id, Uint8 * str)
{
	Uint32 i;
	for(i=0; str[i]; i++)
		UART_putc(id, str[i]);
}

void UART_send_datas(UART_id_e id, Uint8 * datas, Uint32 len)
{
	Uint32 i;
	for(i=0; i<len; i++)
		UART_putc(id, datas[i]);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static bool_e UART_process_irq(UART_id_e id)
{
	UART_handler_t * h;
	bool_e ret = FALSE;
	USART_TypeDef * usart;
	usart = (USART_TypeDef *)uarts[id];
	h = (UART_handler_t *)&handlers[id];
	uint16_t reg;
	reg = usart->SR & usart->CR1;
	if(reg & 0x20)		//équivaut à USART_GetITStatus(usart, USART_IT_RXNE)
	{
		Uint8 c;
		while(usart->SR & USART_FLAG_RXNE)	//équivaut à USART_GetFlagStatus(usart, USART_FLAG_RXNE)
		{
			c = usart->DR;	//équivaut à : USART_ReceiveData(usart);
			if(h->initialized)
			{
				h->prxbuf[h->rx_index_in % RX_BUFFER_SIZE] = c;
				h->rx_index_in++;
				if(h->callback_receive)
					h->callback_receive(c);
				ret = TRUE;
			}
		}
	}
	if(reg & 0x80)	//équivaut à USART_GetITStatus(usart, USART_IT_TXE)
	{
		Uint32 in, out;
		in = h->tx_index_in;
		out = h->tx_index_out;
		if(in-out)
		{	//il reste des données à envoyer
			usart->DR = h->ptxbuf[out % TX_BUFFER_SIZE];	//équivant à USART_SendData(usart, c); avec c = h->ptxbuf[out % TX_BUFFER_SIZE];
			h->tx_index_out = out + 1;
		}
		else
		{	//buffer vide -> Plus rien à envoyer -> désactivation de l'IT TXE.
			usart->CR1 &= ~0x80;	//équivaut à USART_ITConfig(usart, USART_IT_TXE, DISABLE);
		}
	}
	if((usart->SR & 0x08) && (usart->CR1 & 0x20)){	// équivaut à USART_GetITStatus(usart, USART_IT_ORE_RX)
		volatile Uint8 c = usart->DR;	//équivaut à : USART_ReceiveData(usart);
		UNUSED_VAR(c);
	}
	return ret;
}

#if USE_UART1
	void USART1_IRQHandler(void)
	{
		UART_process_irq(UART1_ID);
	}
#endif

#if USE_UART2
	void USART2_IRQHandler(void)
	{
		UART_process_irq(UART2_ID);
	}
#endif

#if USE_UART3
	void USART3_IRQHandler(void)
	{
		UART_process_irq(UART3_ID);
	}
#endif

#if USE_UART4
	void UART4_IRQHandler(void)
	{
		UART_process_irq(UART4_ID);
	}
#endif

#if USE_UART5
	void UART5_IRQHandler(void)
	{
		UART_process_irq(UART5_ID);
	}
	#endif

#if USE_UART6
	void USART6_IRQHandler(void)
	{
		UART_process_irq(UART6_ID);
	}
#endif


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

char UART_read_stdin(void)
{
	while(!UART_data_ready(uart_for_stdin_id));
	return (char)UART_get_next_msg(uart_for_stdin_id);
}

void UART_write_stdout(char c)
{
	UART_putc(uart_for_stdout_id,(uint8_t)c);
}

void UART_write_sterr(char c)
{
	while((uarts[uart_for_stderr_id]->SR & USART_FLAG_TXE) == RESET);
	uarts[uart_for_stderr_id]->DR = (c & (uint16_t)0x01FF);
}

void UART_printf(UART_id_e uart_id, const char * format, ...)
{
	static char buffer[512]; //static pour être sur d'avoir la mémoire dispo, 512 pourrait causer des débordements de pile ?
	va_list args_list;
	va_start(args_list, format);
	vsnprintf(buffer, 512, format, args_list);
	va_end(args_list);

	UART_puts(uart_id, buffer);
}


