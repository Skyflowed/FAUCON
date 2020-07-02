/*
 * datas_process.h

 *
 *  Created on: 23 juin 2020
 *      Author: Arnaud
 */

#ifndef APPLI_DATAS_PROCESS_H_
#define APPLI_DATAS_PROCESS_H_

#include "tests_methods.h"

MPU6050_t mpu_datas_res;
bool_e mpu_init_OK ;

typedef struct{
	int16_t Accelerometer_X; /*!< Accelerometer value X axis */
	int16_t Accelerometer_Y; /*!< Accelerometer value Y axis */
	int16_t Accelerometer_Z; /*!< Accelerometer value Z axis */
	int16_t Gyroscope_X;     /*!< Gyroscope value X axis */
	int16_t Gyroscope_Y;     /*!< Gyroscope value Y axis */
	int16_t Gyroscope_Z;     /*!< Gyroscope value Z axis */
}datas_drone_movement_t;

typedef struct{
	uint16_t dist_1;
	uint16_t dist_2;
	uint16_t dist_3;
	uint16_t dist_4;
	uint16_t dist_5;

	uint16_t angle_X;
	uint16_t angle_Y;
	uint16_t angle_Z;
}datas_drone_position_t;

datas_drone_position_t datas_drone_position;
datas_drone_movement_t datas_drone_movement;

void TIMER2_user_handler_it_1ms(void);
void com_datas_tof_maj();
void com_datas_gyro_maj();
void com_datas_acc_maj();
void send_datas_tof_bluetooth();













#endif /* APPLI_DATAS_PROCESS_H_ */
