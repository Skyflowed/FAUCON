/*
 * datas_process.c

 *
 *  Created on: 23 juin 2020
 *      Author: Arnaud
 */


#include "datas_process.h"


/// INTERROGATION AVEC IT DE TIMER 2   ///
<<<<<<< Updated upstream

//IT 1ms, de plus basse priorité que l'IT du systick !
void TIMER2_user_handler_it_1ms(void)
{
//compteur declaré dans tests_methods.h
	if(compt <= 5000){
		compt += 1;
	}
	else{
		compt = 0;
=======
//IT 1ms, de plus basse prioritï¿½ que l'IT du systick !
void TIMER2_user_handler_it_1ms(void)
{
//compteur declarï¿½ dans tests_methods.h
	if(compt <= 5000){ compt += 1; }
	else{ compt = 0; }

//IDEE => Fonction d'interrogation des ToFs toutes les TIME_NO_POOLING_TOF ms, si en communication TOF_OK = 0, sinon, TOF_OK = 1
//compteur declarï¿½ dans datas_process.h
	static timeslot_e timeslot;

	if(compteur_no_pooling_tof <= TIME_NO_POOLING_TOF && TOF_OK ==1){
		compteur_no_pooling_tof++;
		TOF_OK = 1;
>>>>>>> Stashed changes
	}


	timeslot_e timeslot;

	//send_datas_tof_bluetooth();


	//timeslot = VL53L1X_process_it();

<<<<<<< Updated upstream
	//test_gyro(mpu_test_results);
	//switch(timeslot)... //si l'on souhaite conditionner certaines choses aux timeslots des VL_53... pour répartir la charge de calcul dans le temps.
=======
	//switch(timeslot)... //si l'on souhaite conditionner certaines choses aux timeslots des VL_53... pour rï¿½partir la charge de calcul dans le temps.
>>>>>>> Stashed changes

	//notamment pour discuter avec le MPU6050 ! (car il utilise le mï¿½me bus I2C ! donc il faut uniquement lui causer en IT !)
}


<<<<<<< Updated upstream
void com_datas_tof_maj(){
	datas_drone_position.dist_1 = VL53L1X_get_distance(0);
	//datas_tof.dist_2 = VL53L1X_get_distance(1);
	//datas_tof.dist_3 = VL53L1X_get_distance(2);
	//datas_tof.dist_4 = VL53L1X_get_distance(3);
	//datas_tof.dist_5 = VL53L1X_get_distance(4);
=======
uint8_t data_process_main(){
	static state_e state;
	state = INIT_POSITION;
	static uint8_t DATAS_STRUCT_POSITION_OK = 0;

	switch (state) {
		case INIT_POSITION:
			//.................init()
			state = READ_AND_PROCESS_DATA;
			break;
		case READ_AND_PROCESS_DATA:
			if(TOF_OK){
				//maj nos datastrcut rï¿½elles dans datas_sensors_pooling
				datas_gyro_maj(&mpu_datas_res);//com avec MPU
				datas_tof_maj();
				process_data();
			}
			//state = PROCESS_DATA;
			DATAS_STRUCT_POSITION_OK = 1;

			break;
		case STOP :
			break;


	}

	return DATAS_STRUCT_POSITION_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void datas_tof_maj(){
	datas_sensors_pooling.dist_0 = VL53L1X_get_distance(0);
	datas_sensors_pooling.dist_1 = VL53L1X_get_distance(1);
	datas_sensors_pooling.dist_2 = VL53L1X_get_distance(2);
	datas_sensors_pooling.dist_3 = VL53L1X_get_distance(3);
	datas_sensors_pooling.dist_4 = VL53L1X_get_distance(4);
>>>>>>> Stashed changes
}

void com_datas_gyro_maj(){
	if(mpu_init_OK != FALSE){
		//Alimente gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, SET);

		MPU6050_ReadAll(&mpu_datas_res);

		//eteint gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, RESET);
	}

<<<<<<< Updated upstream
	datas_drone_movement.Gyroscope_X += mpu_datas_res.Gyroscope_X;
	datas_drone_movement.Gyroscope_Y += mpu_datas_res.Gyroscope_Y;
	datas_drone_movement.Gyroscope_Z += mpu_datas_res.Gyroscope_Z;

	//divise par 16400 pour avoir environ en degrés
	datas_drone_movement.Gyroscope_X /= 16400;
	datas_drone_movement.Gyroscope_Y /= 16400;
	datas_drone_movement.Gyroscope_Z /= 16400;
=======
//  maj la sruct de datas des mouvements
	datas_sensors_pooling.Gyroscope_X = DataStruct->Gyroscope_X;
	datas_sensors_pooling.Gyroscope_Y = DataStruct->Gyroscope_Y;
	datas_sensors_pooling.Gyroscope_Z = DataStruct->Gyroscope_Z;
>>>>>>> Stashed changes
}

void com_datas_acc_maj(){
	if(mpu_init_OK != FALSE){
		//Alimente gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, SET);

		MPU6050_ReadAll(&mpu_datas_res);

		//eteint gyro
		HAL_GPIO_WritePin( MPU6050_VCC_GPIO, MPU6050_VCC_PIN, RESET);
	}

<<<<<<< Updated upstream
	datas_drone_movement.Accelerometer_X = mpu_datas_res.Accelerometer_X;
	datas_drone_movement.Accelerometer_Y = mpu_datas_res.Accelerometer_Y;
	datas_drone_movement.Accelerometer_Z = mpu_datas_res.Accelerometer_Z;
=======
	//  maj la sruct de datas des mouvements
	datas_sensors_pooling.Accelerometer_X = DataStruct->Accelerometer_X;
	datas_sensors_pooling.Accelerometer_Y = DataStruct->Accelerometer_Y;
	datas_sensors_pooling.Accelerometer_Z = DataStruct->Accelerometer_Z;

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void process_data(){
	//gyor
	datas_drone_position.angle_X_ROLL += datas_sensors_pooling.Gyroscope_X;
	datas_drone_position.angle_Y_PITCH += datas_sensors_pooling.Gyroscope_Y;
	datas_drone_position.angle_Z_YAW += datas_sensors_pooling.Gyroscope_Z;

	datas_drone_position.angle_X_ROLL /= 16400; //convertir en ï¿½
	datas_drone_position.angle_Y_PITCH /= 16400; //convertir en ï¿½
	datas_drone_position.angle_Z_YAW /= 16400; //convertir en ï¿½

	//tofs
	//datas_drone_position.position_X = datas_sensors_pooling.CAT(dist_, TOF_ID_AXE_X);

	datas_drone_position.position_X = datas_sensors_pooling.dist_0 - datas_drone_position.X0 - MESURE_LARGEUR_DRONE_CM;
	datas_drone_position.position_Y = datas_sensors_pooling.dist_1 - datas_drone_position.Y0 - MESURE_LARGEUR_DRONE_CM;
	datas_drone_position.position_Z = datas_sensors_pooling.dist_2 - datas_drone_position.Z0 - MESURE_HAUEUR_DRONE_CM;


}

void init_datas(){
	datas_drone_position.X0 = 0;
	datas_drone_position.Y0 = 0;
	datas_drone_position.Z0 = 0;


//...................................

>>>>>>> Stashed changes

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









