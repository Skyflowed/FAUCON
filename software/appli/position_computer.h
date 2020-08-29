/*POSITION_COMPUTER.H*/

#ifndef POSITION_COMPUTER_H
	#define POSITION_COMPUTER_H

void PC_process_main(void);
double PC_get_x(void);
double PC_get_y(void);
double PC_get_z(void);
double PC_get_roll(void);
double PC_get_pitch(void);
double PC_get_yaw(void);

#define CENTER_DISTANCE_X	0.85
#define CENTER_DISTANCE_Y	0.92
#define CENTER_DISTANCE_Z	0.85
#define PRECISION			0.1
//#define DEGREES_PER_CM	0.016

//#define BORDER_ANGLE_FACE_LEFT		45
//#define BORDER_ANGLE_FACE_BOTTOM	    135
//#define BORDER_ANGLE_FACE_RIGHT		225
//#define BORDER_ANGLE_FACE_TOP   	    315
//#define BORDER_ANGLE_FACE_FORWARD
//#define BORDER_ANGLE_FACE_BACKWARD


/*
 * 									DRONE VU DE PROFIL (CAPTEUR 1 A L'AVANT)
 *
 *
 * 									     FACE UP
 *          315° -------------------------------------------------------  225°
 * 				|							|							|			1 : Capteur 1	   RAPPEL DIMENSIONS : 30x30x15cm (Lxlxh) --> le 15cm explique le 1.92 du CENTER_DISTANCE_Y
 * 				|							|							|			2 : Capteur 2
 * 				|							|							|			3 : Capteur 3
 * 				|							|							|
 * 				|							|							|
 * 				|							|Y							|
 * 				|							|							|
 * 				|							|							|
 * 				|							|							|
 * 				|							|							|
 * 				|							|							|
 * 	  FACE LEFT	|			Z			|---2--|			Z			|	FACE RIGHT
 * 				|-----------------------|1	3  |------------------------|
 * 				|						|------|						|
 * 				|							|	X(Axe du capteur 3)		|
 * 				|							|		(Vers nous)			|
 * 				|							|							|
 * 				|							|							|
 * 				|							|							|
 * 				|							|							|
 * 				|							|Y							|
 * 				|							|							|
 * 				|							|							|
 * 				|							|							|
 * 				|							|							|
 * 		     45° -------------------------------------------------------  135°
 *									     FACE DOWN
 */

#endif /*POSITION_COMPUTER_H*/
