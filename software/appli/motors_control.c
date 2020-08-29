/**
  ******************************************************************************
  * @file    motors_control.c
  * @author  Flo
  * @date    25-August-2020
  * @brief   Fonctionnalités permettant de gérer l'activation et la vitesse des moteurs
  ******************************************************************************
*/

#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include <stdbool.h>
#include <math.h>
#include "macro_types.h"
#include "motors_control.h"
#include "position_computer.h"

typedef enum
{
	FIXATION_INIT,
	FIXING,
}fixes_e;

typedef enum
{
	NEED_FIXATION = 0,
	FIXATION_IN_PROGRESS,
	FIXATION_OK,
	FIXATION_TIMEOUT, //Au delà de 3 sec on recalcule ?
};

static double roll;
static double pitch;
static double yaw;
static uint8_t ROLL_FIXATION;
static uint8_t PITCH_FIXATION;
static uint8_t YAW_FIXATION;
static void MC_check_angles(void);

void MC_init(void)
{
	ROLL_FIXATION = FALSE;
	PITCH_FIXATION = FALSE;
	YAW_FIXATION = FALSE;
	//Init des moteurs
}

void MC_mode_management(void)
{
	//Faut il tenter de gérer tous les axes uns par uns ou faire axe par axe ? Pitch,Roll,Yaw
	static fixes_e mode = FIXATION_INIT;
	switch(mode)
	{
	case FIXATION_INIT :
		MC_init();
		break;
	case FIXING :
		PC_check_angles();
		if(PITCH_FIXATION == NEED_FIXATION)
		{
			PITCH_FIXATION = FIXATION_IN_PROGRESS;
			if(pitch > MINOR_ANGLE)
			{
				//Combinaison moteurs
			}
			else if(pitch < -MINOR_ANGLE)
			{
				//Combinaison moteurs
			}
		}
		if(ROLL_FIXATION == NEED_FIXATION && PITCH_FIXATION == FIXATION_OK)
		{
			ROLL_FIXATION = FIXATION_IN_PROGRESS;
			if(roll > MINOR_ANGLE)
			{
				//Combinaison moteurs
			}
			else if(roll < -MINOR_ANGLE)
			{
				//Combinaison moteurs
			}
		}
		if(YAW_FIXATION == NEED_FIXATION && (PITCH_FIXATION == ROLL_FIXATION == FIXATION_OK))
		{
			YAW_FIXATION = FIXATION_IN_PROGRESS;
			if(yaw > MINOR_ANGLE)
			{
				//Combinaison moteurs
			}
			else if(yaw < -MINOR_ANGLE)
			{
				//Combinaison moteurs
			}
		}
		break;
	default :
		printf("Error managing motors");
		break;
	}
}

void MC_check_angles(void)
{
	roll = PC_get_roll();
	pitch = PC_get_pitch();
	yaw = PC_get_yaw();
	//ROLL
	if (!(roll>=-MINOR_ANGLE && roll<=MINOR_ANGLE))
	{
		ROLL_FIXATION = NEED_FIXATION;
	}

	else
	{
		ROLL_FIXATION = FIXATION_OK;
	}
	//PITCH
	if (!(pitch>=-MINOR_ANGLE && pitch<=MINOR_ANGLE))
	{
		PITCH_FIXATION = NEED_FIXATION;
	}

	else
	{
		PITCH_FIXATION = FIXATION_OK;
	}
	//YAW
	if (!(yaw>=-MINOR_ANGLE && yaw<=MINOR_ANGLE))
	{
		YAW_FIXATION = NEED_FIXATION;
	}

	else
	{
		YAW_FIXATION = FIXATION_OK;
	}
}

