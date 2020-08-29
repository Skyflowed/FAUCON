/*
 * datas_process.c

 *
 *  Created on: 23 juin 2020
 *      Author: Arnaud
 */


#include "datas_process.h"


/// INTERROGATION AVEC IT DE TIMER 2   ///

//IT 1ms, de plus basse priorité que l'IT du systick !
void TIMER2_user_handler_it_1ms(void)
{
//compteur declaré dans tests_methods.h
	if(compt <= 5000){
		compt += 1;
	}
	else{
		compt = 0;
	}


	timeslot_e timeslot;

	//send_datas_tof_bluetooth();


	//timeslot = VL53L1X_process_it();

	//test_gyro(mpu_test_results);
	//switch(timeslot)... //si l'on souhaite conditionner certaines choses aux timeslots des VL_53... pour répartir la charge de calcul dans le temps.

	//notamment pour discuter avec le MPU6050 ! (car il utilise le même bus I2C ! donc il faut uniquement lui causer en IT !)
}


void com_datas_tof_maj(){
	datas_drone_position.dist_1 = VL53L1X_get_distance(0);
	//datas_tof.dist_2 = VL53L1X_get_distance(1);
	//datas_tof.dist_3 = VL53L1X_get_distance(2);
	//datas_tof.dist_4 = VL53L1X_get_distance(3);
	//datas_tof.dist_5 = VL53L1X_get_distance(4);
}

void com_datas_gyro_maj(){
	if(mpu_init_OK != FALSE){
		//Alimente gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, SET);

		MPU6050_ReadAll(&mpu_datas_res);

		//eteint gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, RESET);
	}

	datas_drone_movement.Gyroscope_X += mpu_datas_res.Gyroscope_X;
	datas_drone_movement.Gyroscope_Y += mpu_datas_res.Gyroscope_Y;
	datas_drone_movement.Gyroscope_Z += mpu_datas_res.Gyroscope_Z;

	//divise par 16400 pour avoir environ en degrés
	datas_drone_movement.Gyroscope_X /= 16400;
	datas_drone_movement.Gyroscope_Y /= 16400;
	datas_drone_movement.Gyroscope_Z /= 16400;
}

void com_datas_acc_maj(){
	if(mpu_init_OK != FALSE){
		//Alimente gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, SET);

		MPU6050_ReadAll(&mpu_datas_res);

		//eteint gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, RESET);
	}

	datas_drone_movement.Accelerometer_X = mpu_datas_res.Accelerometer_X;
	datas_drone_movement.Accelerometer_Y = mpu_datas_res.Accelerometer_Y;
	datas_drone_movement.Accelerometer_Z = mpu_datas_res.Accelerometer_Z;

	//divise par 420 pour avoir environ un %
	datas_drone_movement.Gyroscope_X /= 420;
	datas_drone_movement.Gyroscope_Y /= 420;
	datas_drone_movement.Gyroscope_Z /= 420;
}


void send_datas_tof_bluetooth(){
	if(compt == 1000){
		//UART_puts(UART2_ID, "tof_1: ", 7);
		//envoiCaractere(datas_tof.dist_1);

		envoiCaractere('g');
	}
}









