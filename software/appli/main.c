 /**
  *
  * \file main.c
  */

/* Includes ------------------------------------------------------------------*/

#include <vl53l1x/vl53l1x.h>
#include "stm32f4xx_hal.h"
#include "macro_types.h"
#include "main.h"
#include "stm32f4_uart.h"
#include "stm32f4_sys.h"
#include "stm32f4_gpio.h"
#include "stm32f4_timer.h"
#include "MPU6050/stm32f4_mpu6050.h"
#include "systick.h"
#include "init_stm32f4.h"
#include "pixy.h"
#include "bluetooth.h"




static uint32_t compt;


int main(void){
	HAL_Init();
	SYS_init();			//initialisation du systeme (horloge...)

	GPIO_Configure();
	UART_init(UART6_ID, 38400);
	SYS_set_std_usart(UART6_ID, UART6_ID, UART6_ID);


	BSP_GPIO_PinCfg(LEDS_GPIO, LED_GREEN_PIN | LED_ORANGE_PIN | LED_RED_PIN | LED_BLUE_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FAST, 0);
	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FAST, 0);

	Systick_init();

	//HAL_GPIO_WritePin(LEDS_GPIO, LED_GREEN_PIN | LED_ORANGE_PIN, SET);
	//HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN | LED_BLUE_PIN, SET);

	//pixy_init();

	//init_pwm_tim1_tim3();

	//TEST_pwms();
	//test_moteur_PC6(0);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET);

	VL53L1X_init();

	/*
	MPU6050_t mpu6050_datastruct;
	bool_e mpu6050_initialized = FALSE;
	if(MPU6050_Init(&mpu6050_datastruct, MPU6050_VCC_GPIO, MPU6050_VCC_PIN, MPU6050_Device_0, MPU6050_Accelerometer_8G, MPU6050_Gyroscope_2000s) == MPU6050_Result_Ok)
		mpu6050_initialized = TRUE;
	*/

	TIMER2_run_1ms();

	static uint16_t dist0 ;
	static uint16_t dist1 ;


	while (1)
	{
/*		uint8_t a = UART_get_next_byte(UART6_ID);
		if(a == 'a'){
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, SET);
		}

*/
/*
		//BLUETOOTH
		if(compt < 2000 && compt >= 0){
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_BLUE_PIN, RESET);
			envoiCaractere('c');
		}
		else if(compt < 4000 && compt > 2000){
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, RESET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_BLUE_PIN, SET);
			envoiCaractere('a');
		}
		else if(compt < 6000 && compt > 4000){
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_BLUE_PIN, RESET);
			envoiTrame("martin");
		}
		else if(compt > 6000 && compt < 8000){
			HAL_GPIO_WritePin(LEDS_GPIO, LED_BLUE_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, SET);
			compt = 0;
		}
*/

		//MOTEURS
/*
		if(compt < 3000 && compt >= 0){
			test_moteur_PC6(55);
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_BLUE_PIN, RESET);

		}
		else if(compt < 6000 && compt > 3000){
			test_moteur_PC6(57);
			HAL_GPIO_WritePin(LEDS_GPIO, LED_BLUE_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, RESET);
		}
		else if(compt < 9000 && compt > 6000){
			test_moteur_PC6(59);
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_BLUE_PIN, RESET);
		}
		else if(compt < 12000 && compt > 9000){
			test_moteur_PC6(61);
			HAL_GPIO_WritePin(LEDS_GPIO, LED_BLUE_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, RESET);
		}
		else if(compt < 15000 && compt > 12000){
			test_moteur_PC6(63);
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_BLUE_PIN, RESET);
		}
		else if(compt < 18000 && compt > 15000){
			test_moteur_PC6(65);
			HAL_GPIO_WritePin(LEDS_GPIO, LED_BLUE_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, RESET);
		}
		else if(compt > 18000 && compt < 23000){
			HAL_GPIO_WritePin(LEDS_GPIO, LED_BLUE_PIN, RESET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, RESET);
			compt = 0;
		}
*/

		//TOF
		dist0 = VL53L1X_get_distance(0);
		dist1 = VL53L1X_get_distance(1);

		if(dist0<100 && dist0 > 0){
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, SET);
		}
		else{
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, RESET);
		}

		if(dist1<100 && dist1 > 0){
			HAL_GPIO_WritePin(LEDS_GPIO,LED_GREEN_PIN, SET);
		}
		else{
			HAL_GPIO_WritePin(LEDS_GPIO,LED_GREEN_PIN, RESET);
		}

		//pixy_process_main();
	}
}

//IT 1ms, de plus basse priorité que l'IT du systick !
void TIMER2_user_handler_it_1ms(void)
{

	compt += 1;

	timeslot_e timeslot;
	timeslot = VL53L1X_process_it();

	//switch(timeslot)... //si l'on souhaite conditionner certaines choses aux timeslots des VL_53... pour répartir la charge de calcul dans le temps.

	//notamment pour discuter avec le MPU6050 ! (car il utilise le même bus I2C ! donc il faut uniquement lui causer en IT !)
}


