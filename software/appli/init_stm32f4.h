/*
 * init_stm32f4.h
 *
 *  Created on: 30 mai 2020
 *      Author: Arnaud
 */

#ifndef APPLI_INIT_STM32F4_H_
#define APPLI_INIT_STM32F4_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "macro_types.h"
#include "stm32f4_uart.h"
#include "stm32f4_sys.h"
#include "stm32f4_spi.h"
#include "stm32f4_gpio.h"
#include "stm32f4_timer.h"



#define PWM_MOT_11 GPIOC, GPIO_PIN_6
#define PWM_MOT_12 GPIOC, GPIO_PIN_7
#define PWM_MOT_13 GPIOC, GPIO_PIN_8
#define PWM_MOT_14 GPIOC, GPIO_PIN_9

#define PWM_MOT_21 GPIOE, GPIO_PIN_9
#define PWM_MOT_22 GPIOE, GPIO_PIN_11
#define PWM_MOT_23 GPIOE, GPIO_PIN_13
#define PWM_MOT_24 GPIOE, GPIO_PIN_14

void init_pwm_tim1_tim3(void);
void PWM_TIMER_set_duty(TIM_HandleTypeDef tim_handle, int channel, uint16_t duty);
void TEST_pwms();
void test_moteur_PC6(uint16_t TIME);





#endif /* APPLI_INIT_STM32F4_H_ */
