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
#include "tests_methods.h"
#include "datas_process.h"


int main(void){
	HAL_Init();
	SYS_init();			//initialisation du systeme (horloge...)

	GPIO_Configure();
	//UART DU BLUETOOTH
	UART_init(UART2_ID, 38400);
	//UNI
	//SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);


	BSP_GPIO_PinCfg(LEDS_GPIO, LED_GREEN_PIN | LED_ORANGE_PIN | LED_RED_PIN | LED_BLUE_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FAST, 0);
	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FAST, 0);

	Systick_init();

	//INIT DES CAPTEURS
	//pixy_init();
	init_pwm_tim1_tim3();
	//VL53L1X_init();

	//MPU6050_t mpu6050_datastruct;
	//bool_e mpu6050_initialized = FALSE;

	if(MPU6050_Init(&mpu_datas_res, MPU6050_VCC_GPIO, MPU6050_VCC_PIN, MPU6050_Device_0, MPU6050_Accelerometer_8G, MPU6050_Gyroscope_2000s) == MPU6050_Result_Ok)
		mpu_init_OK = TRUE;

	mpu_test_OK = mpu_init_OK;
	mpu_test_results = mpu_datas_res;

	TIMER2_run_1ms();

	while (1)
	{
		//puiss_mot_test_PC6();
		test_moteur_PC6(60);

		//pixy_process_main();
	}
}












