/*
 * tests_methods.h

 *
 *  Created on: 23 juin 2020
 *      Author: Arnaud
 */

#ifndef APPLI_TESTS_METHODS_H_
#define APPLI_TESTS_METHODS_H_


#include "main.h"
#include "bluetooth.h"
#include "init_stm32f4.h"
#include <vl53l1x/vl53l1x.h>
#include "MPU6050/stm32f4_mpu6050.h"


//Compteur incrémenté ttes les ms par le TIM2 => pratique pour les tests
uint32_t compt;


MPU6050_t mpu_test_results;

bool_e mpu_test_OK ;

int16_t Accelerometer_X; /*!< Accelerometer value X axis */
int16_t Accelerometer_Y; /*!< Accelerometer value Y axis */
int16_t Accelerometer_Z; /*!< Accelerometer value Z axis */
int16_t Gyroscope_X;     /*!< Gyroscope value X axis */
int16_t Gyroscope_Y;     /*!< Gyroscope value Y axis */
int16_t Gyroscope_Z;     /*!< Gyroscope value Z axis */



void test_ble();
void puiss_mot_test_PC6();
void test_tof_1_et_2();
void test_gyro(MPU6050_t mpu_d);



















#endif /* APPLI_TESTS_METHODS_H_ */
