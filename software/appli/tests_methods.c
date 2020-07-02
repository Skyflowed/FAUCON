/*
 * tests_methods.c

 *
 *  Created on: 23 juin 2020
 *      Author: Arnaud
 */


#include "tests_methods.h"



///   BLUETOOTH   ///
void test_ble(){

    	uint8_t a = UART_get_next_byte(UART6_ID);
		if(a == 'a'){
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, SET);
		}

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
			//envoiTrame("martin");
		}
		else if(compt > 6000 && compt < 8000){
			HAL_GPIO_WritePin(LEDS_GPIO, LED_BLUE_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, SET);
			compt = 0;
		}
}


///   MOTEURS PWM   ///
void puiss_mot_test_PC6(){
		if(compt < 3000 && compt >= 0){
			test_moteur_PC6(55);
			HAL_GPIO_WritePin(LEDS_GPIO, LED_RED_PIN, SET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_BLUE_PIN, RESET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_GREEN_PIN, RESET);

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
		else if(compt > 18000 && compt < 20000){
			test_moteur_PC6(0);

			HAL_GPIO_WritePin(LEDS_GPIO, LED_BLUE_PIN, RESET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_RED_PIN, RESET);
			HAL_GPIO_WritePin(LEDS_GPIO,LED_GREEN_PIN, SET);
		}
}


///   MOTEURS PWM   ///
void test_tof_1_et_2(){

		uint16_t dist0 ;
		uint16_t dist1 ;

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
}


///   GYRO   ///
void test_gyro(MPU6050_t mpu_d){
	if(mpu_test_OK != FALSE){
		//Alimente gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, SET);

		MPU6050_ReadAll(&mpu_test_results);

		//eteint gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, RESET);
	}

	Gyroscope_X += mpu_d.Gyroscope_X;
	Gyroscope_Y += mpu_d.Gyroscope_Y;
	Gyroscope_Z += mpu_d.Gyroscope_Z;

	Gyroscope_X /= 16400;
	Gyroscope_Y /= 16400;
	Gyroscope_Z /= 16400;
}











