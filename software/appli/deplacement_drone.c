/*
 * deplacement_drone.c
 *
 *  Created on: 30 juin 2020
 *      Author: Arnaud
 */

//==================================================================================================
/*
Pour les d�placements du drone, on distingue :
- Translations sur axe Z
- Translations sur axe X et Y
- Rotation sur les axes X et Y
- Rotation sur l'axe Z

====>>>>>> ATTENTION A L'INERTIE !!!!! <<<<<<<=====
==>> TRANSLATIONS : il faut absolument v�rifier que le drone ait une position angulaire nulle avant d'effectuer une translation
==>> Prendre en compte

Translation sur Z:
Moteurs de face haute ou basse.

Translation sur X et Y:
Selon l'axe X ou Y ; rotation de 90�, mise en marche de tous les moteurs de la face basse ou haute, dur�e/puissance selon la distance � parcourir, puis rotation de -90�.


Rotation sur X et Y:
Utilisation de tous les moteurs pour effectuer une rotation ; Algo de calcul � impl�menter (check controlleur de vol de drone de commerce),

Rotation sur Z:
Utilisation de tous les moteurs de chaque face.

---------------------------------------

Rappels des moterus :
 PWM_MOT_11 GPIOC, GPIO_PIN_6 => TIM3_CH1
 PWM_MOT_12 GPIOC, GPIO_PIN_7 => TIM3_CH2
 PWM_MOT_13 GPIOC, GPIO_PIN_8 => TIM3_CH3
 PWM_MOT_14 GPIOC, GPIO_PIN_9 => TIM3_CH4

 PWM_MOT_21 GPIOE, GPIO_PIN_9 => TIM1_CH1
 PWM_MOT_22 GPIOE, GPIO_PIN_11 => TIM1_CH2
 PWM_MOT_23 GPIOE, GPIO_PIN_13 => TIM1_CH3
 PWM_MOT_24 GPIOE, GPIO_PIN_14 => TIM1_CH4

*/
//==================================================================================================


#include "deplacement_drone.h"
#include "stdbool.h"

static double roll;
static double pitch;
static double yaw;
static uint8_t ROLL_FIXATION;
static uint8_t PITCH_FIXATION;
static uint8_t YAW_FIXATION;
static bool STABILISATION_OK;
static void DD_check_angles(void);

//FONCTIONS DE STABILISATION
void DD_init(void)
{
	roll = 0;
	pitch = 0;
	yaw = 0;
	ROLL_FIXATION = FALSE;
	PITCH_FIXATION = FALSE;
	YAW_FIXATION = FALSE;
	STABILISATION_OK = FALSE;
	//Init des moteurs
}

void DD_check_angles(void)
{
	roll = datas_drone_position.angle_X_ROLL;
	pitch = datas_drone_position.angle_Y_PITCH;
	yaw = datas_drone_position.angle_Z_YAW;
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

void DD_mode_management(void)
{
	static fixes_e mode = FIXATION_INIT;
	switch(mode)
	{
	case FIXATION_INIT :
		DD_init();
		break;
	case FIXING :
		DD_check_angles();
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
			break;
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
			break;
		}
		if(YAW_FIXATION == NEED_FIXATION && PITCH_FIXATION == FIXATION_OK && ROLL_FIXATION == FIXATION_OK)
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
			break;
		}
		mode = FIXED;
		break;
	case FIXED :
		STABILISATION_OK = TRUE;
		break;
	default :
		printf("Error stabilising the drone");
		break;
	}
}
// FONCTIONS DE TRANSLATION
void trans_simple_Z(trans_z_e direction, uint8_t time){
	if(direction == TRANS_Z_BAS){
		//moteurs haut pr aller vers bas
	}
	else if (direction == TRANS_Z_HAUT) {
		//moteurs bas pr aller vers haut
	}

}




<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
