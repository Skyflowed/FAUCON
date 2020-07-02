/*
 * deplacement_drone.c
 *
 *  Created on: 30 juin 2020
 *      Author: Arnaud
 */

//==================================================================================================
/*
Pour les déplacements du drone, on distingue :
- Translations sur axe Z
- Translations sur axe X et Y
- Rotation sur les axes X et Y
- Rotation sur l'axe Z

====>>>>>> ATTENTION A L'INERTIE !!!!! <<<<<<<=====
==>> TRANSLATIONS : il faut absolument vérifier que le drone ait une position angulaire nulle avant d'effectuer une translation
==>> Prendre en compte

Translation sur Z:
Moteurs de face haute ou basse.

Translation sur X et Y:
Selon l'axe X ou Y ; rotation de 90°, mise en marche de tous les moteurs de la face basse ou haute, durée/puissance selon la distance à parcourir, puis rotation de -90°.


Rotation sur X et Y:
Utilisation de tous les moteurs pour effectuer une rotation ; Algo de calcul à implémenter (check controlleur de vol de drone de commerce),

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



void trans_simple_Z(trans_z_e direction, uint8_t time){
	if(direction == TRANS_Z_BAS){
		//moteurs haut pr aller vers bas
	}
	else if (direction == TRANS_Z_HAUT) {
		//moteurs bas pr aller vers haut
	}

}




