/*
 * init_stm32f4.c

 *
 *  Created on: 30 mai 2020
 *      Author: Arnaud
 */

#include "init_stm32f4.h"


///*** PORTS INIT ***///

//PWM
//MACROS DE Mr POIRAUDS
#define TIM1_8_9_10_11_CLK    			168000000	//Fréquence des évènements d'horloge pour les timers 1, 8, 9, 10, 11
#define TIM2_3_4_5_6_7_12_13_14_CLK    	84000000	//Fréquence des évènements d'horloge pour les timers 2, 3, 4, 5, 6, 7, 12, 13 et 14

//L'horloge du timer 3 est a 84MHz
//Si l'on veut une PWM a 20kHz (inaudible) et 100 pas de rapports cycliques possibles, il faut prediviser par 42 :
//168MHz/84 = 2MHz -> 500ns par pas... * 100 pas = 20kHz de frequence PWM
#define PWM_FREQ_TIM	500 	//Fréquence du signal PWM, en Hz
#define	PWM_PERIOD_TIM	100		//Nombre jusqu'auquel le timer va compter durant une période PWM

#define	PWM_PRESC_TIM_3	((TIM2_3_4_5_6_7_12_13_14_CLK / PWM_FREQ_TIM) / PWM_PERIOD_TIM)	//Prédiviseur : nombre d'évènements qui provoquent +1 sur le décompte du timer
#define	PWM_PRESC_TIM_1	((TIM1_8_9_10_11_CLK / PWM_FREQ_TIM) / PWM_PERIOD_TIM)	//Prédiviseur : nombre d'évènements qui provoquent +1 sur le décompte du timer

/*
 * Explications :
 * - Le timer compte des évènements...
 * - Nous disposons d'une horloge à 84MHz...
 * - Tout les X évènements (les X périodes d'horloge), le timer compte +1.... Ce X est le prédiviseur. (PWM_PRESC)
 * - Le timer va compter jusqu'à "PWM_PERIOD"... Puis revenir à 0. Et ainsi de suite.
 * - La durée qu'il met à compter jusqu'à cette PWM_PERIOD nous donne la période du signal PWM... L'inverse est la fréquence !
 * 				Période du signal PWM 	= (PWM_PERIOD	*	PWM_PRESC)/84MHz
 * 	Exemple :				50µs	 	= (100			*	42)/84MHz
 * 	 			Fréquence du signal PWM = 84MHz/(PWM_PERIOD	*	PWM_PRESC)
 * 	Exemple :				50µs	 	= 84MHz/(100		*	42)
 */


static TIM_HandleTypeDef TimHandle_1;	//Structure contenant les infos concernant l'état du timer 1
static TIM_HandleTypeDef TimHandle_3;	//Structure contenant les infos concernant l'état du timer 3

void init_pwm_tim1_tim3(void){

	TimHandle_1.Instance = TIM1;
	TimHandle_3.Instance = TIM3;
	TIM_OC_InitTypeDef TIM_OCInitStruct;

	// mode AF (Alternate Function)
	BSP_GPIO_PinCfg(PWM_MOT_11,GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF2_TIM3);
	BSP_GPIO_PinCfg(PWM_MOT_12,GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF2_TIM3);
	BSP_GPIO_PinCfg(PWM_MOT_13, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF2_TIM3);
	BSP_GPIO_PinCfg(PWM_MOT_14,GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF2_TIM3);

	BSP_GPIO_PinCfg(PWM_MOT_21, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF1_TIM1);
	BSP_GPIO_PinCfg(PWM_MOT_22, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF1_TIM1);
	BSP_GPIO_PinCfg(PWM_MOT_23, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF1_TIM1);
	BSP_GPIO_PinCfg(PWM_MOT_24, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF1_TIM1);

	HAL_GPIO_WritePin(PWM_MOT_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PWM_MOT_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PWM_MOT_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PWM_MOT_14, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(PWM_MOT_21, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PWM_MOT_22, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PWM_MOT_23, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PWM_MOT_24, GPIO_PIN_RESET);

	//autorisations des clocks TIM 1 et 3
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();

	TimHandle_1.Init.Period = PWM_PERIOD_TIM - 1; //0 à periode
	TimHandle_1.Init.Prescaler = PWM_PRESC_TIM_1 - 1;
	TimHandle_1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle_1.Init.CounterMode = TIM_COUNTERMODE_UP;

	TimHandle_3.Init.Period = PWM_PERIOD_TIM - 1; //0 à periode
	TimHandle_3.Init.Prescaler = PWM_PRESC_TIM_3 - 1;
	TimHandle_3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle_3.Init.CounterMode = TIM_COUNTERMODE_UP;

	//init des timer
	HAL_TIM_Base_Init(&TimHandle_1);
	HAL_TIM_Base_Init(&TimHandle_3);

	TIM_OCInitStruct.OCMode = TIM_OCMODE_PWM1;
	TIM_OCInitStruct.Pulse = 0;
	TIM_OCInitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OCInitStruct.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	TIM_OCInitStruct.OCFastMode = TIM_OCFAST_DISABLE;

	//Channels 1,2,3,4 des timers 1 et 3
	if(HAL_TIM_OC_Init(&TimHandle_1) != HAL_OK){

	}
	HAL_TIM_OC_ConfigChannel(&TimHandle_1, &TIM_OCInitStruct, TIM_CHANNEL_1 );
	HAL_TIM_OC_ConfigChannel(&TimHandle_1, &TIM_OCInitStruct, TIM_CHANNEL_2 );
	HAL_TIM_OC_ConfigChannel(&TimHandle_1, &TIM_OCInitStruct, TIM_CHANNEL_3 );
	HAL_TIM_OC_ConfigChannel(&TimHandle_1, &TIM_OCInitStruct, TIM_CHANNEL_4 );

	__HAL_TIM_ENABLE(&TimHandle_1);
	HAL_TIM_PWM_Start(&TimHandle_1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TimHandle_1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TimHandle_1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&TimHandle_1, TIM_CHANNEL_4);


	//ou TIM_CHANNEL_ALL...=> tests

	if(HAL_TIM_OC_Init(&TimHandle_3) != HAL_OK){

	}
	HAL_TIM_OC_ConfigChannel(&TimHandle_3, &TIM_OCInitStruct, TIM_CHANNEL_1 );
	HAL_TIM_OC_ConfigChannel(&TimHandle_3, &TIM_OCInitStruct, TIM_CHANNEL_2 );
	HAL_TIM_OC_ConfigChannel(&TimHandle_3, &TIM_OCInitStruct, TIM_CHANNEL_3 );
	HAL_TIM_OC_ConfigChannel(&TimHandle_3, &TIM_OCInitStruct, TIM_CHANNEL_4 );

	__HAL_TIM_ENABLE(&TimHandle_3);
	HAL_TIM_PWM_Start(&TimHandle_3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TimHandle_3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TimHandle_3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&TimHandle_3, TIM_CHANNEL_4);


	//mise à 0 de tous les timers
	PWM_TIMER_set_duty(TimHandle_1, TIM_CHANNEL_1, 0);
	PWM_TIMER_set_duty(TimHandle_1, TIM_CHANNEL_2, 0);
	PWM_TIMER_set_duty(TimHandle_1, TIM_CHANNEL_3, 0);
	PWM_TIMER_set_duty(TimHandle_1, TIM_CHANNEL_4, 0);

	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_1, 0);
	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_2, 0);
	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_3, 0);
	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_4, 0);

	//mettre duty à x*PWM_PERIOD_TIM avec x entre 0 et 1 !

}

void PWM_TIMER_set_duty(TIM_HandleTypeDef tim_handle, int channel, uint16_t duty){
	duty = MIN(PWM_PERIOD_TIM, duty); //duty ne peut etre plus grand que la periode
	__HAL_TIM_SET_COMPARE(&tim_handle, channel, (uint32_t) duty);
}

/*
 * Fonctions de test des pwm/moteurs
 * PWM T=2ms : f=500Hz
 * Rapport entre 50 et 65
 *
 *
 */

void TEST_pwms(){
/*
	PWM_TIMER_set_duty(TimHandle_1, TIM_CHANNEL_1, 25);
	PWM_TIMER_set_duty(TimHandle_1, TIM_CHANNEL_2, 50);
	PWM_TIMER_set_duty(TimHandle_1, TIM_CHANNEL_3, 75);
	PWM_TIMER_set_duty(TimHandle_1, TIM_CHANNEL_4, 100);

	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_1, 25);
	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_2, 50);
	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_3, 75);
	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_4, 100);
*/

	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_1, 75);

}


void test_moteur_PC6(uint16_t TIME){
	PWM_TIMER_set_duty(TimHandle_3, TIM_CHANNEL_1, TIME);
}







