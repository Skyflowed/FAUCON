/**
  ******************************************************************************
  * @file    position_computer.c
  * @author  Flo
  * @date    03-July-2020
  * @brief   Fonctionnalités permettant de calculer la position du drone dans la cabine
  ******************************************************************************
*/

#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include <stdbool.h>
#include <math.h>
#include "macro_types.h"
#include "position_computer.h"

typedef struct
{
	double axe_x;
	double axe_y;
	double axe_z;
	double roll_angle;
	double pitch_angle;
	double yaw_angle;
}drone_coordinates;

typedef struct
{
	double roll_axe_x;
	double roll_axe_y;
	double roll_axe_z;
	double pitch_axe_x;
	double pitch_axe_y;
	double pitch_axe_z;
	double yaw_axe_x;
	double yaw_axe_y;
	double yaw_axe_z;
}intermediate_struct;

typedef enum
	{
		INIT,
		COMPUTING_ANGLES,
		COMPUTING_DISTANCES,
		COMPUTING_ROLL_DIST,
		COMPUTING_PITCH_DIST,
		COMPUTING_YAW_DIST,
		COMPUTING_FINAL_POS,
	}state_e;

typedef enum
	{
		COMPUTING_INIT,
		SAME_FIRST_SITUATION,
		SECOND_SITUATION,
	}situation_e;


static drone_coordinates pos_struct;
static intermediate_struct int_struct;
static double distance_sensor_1;
static double distance_sensor_2;
static double distance_sensor_3;
static uint8_t new_mode;
static bool modify_mode;
static state_e state;

/***********PRIVATES FUNCTIONS PROTOTYPES************/
static void PC_init(void);
static void PC_compute_position_mode_management(void);
static void PC_compute_position(void);
static void PC_compute_angles(void);
static void PC_compute_distances(void);
static void PC_compute_situation_from_angle(void);
static void PC_compute_average(void);
/********************************************/

/**
* @brief Fonction principale du programme qui tourne en tâche de fond
* @pre  Lancer le mode management en premier pour init les var avant de leur affecter des valeurs via les compute_angles/distances
*/
void PC_process_main(void)
{
	//Calcul de la position du drone
	PC_compute_position_mode_management();
	//Calcul des angles (Avec le ou les MPU6050(s))
	PC_compute_angles();
	//Calcul des distances (Avec les TOFs)
	PC_compute_distances();
	//DEBUG
	printf("distances : \n");
	printf("Sensor 1 (Z): %f \nSensor 2 (Y): %f \nSensor 3 (X): %f\n",distance_sensor_1,distance_sensor_2,distance_sensor_3);
	printf("pos struct :\n");
	printf("X: %f	\nY: %f	\nZ: %f \n",pos_struct.axe_x,pos_struct.axe_y,pos_struct.axe_z);
	printf("ROLL: %f\n	PITCH: %f\n	YAW: %f \n",pos_struct.roll_angle,pos_struct.pitch_angle,pos_struct.yaw_angle);
	//printf("int struct :\n");
	//printf("roll_x: %f roll y: %f roll z: %f \n",int_struct.roll_axe_x,int_struct. roll_axe_y,int_struct.roll_axe_z);
	//printf("pitch_x: %f pitch y: %f pitch z: %f \n",int_struct.pitch_axe_x,int_struct. pitch_axe_y,int_struct.pitch_axe_z);
	//printf("yaw_x: %f yaw y: %f yaw z: %f \n",int_struct.yaw_axe_x,int_struct. yaw_axe_y,int_struct.yaw_axe_z);
}

/**
* @brief Initialise la structure contenant les données de la position du drone
* @pre   A executer dans le main avant la while(1)
*/
void PC_init(void)
{
	state = INIT;
	//init de la structure de position
	pos_struct.axe_x = 0.0;
	pos_struct.axe_y = 0.0;
	pos_struct.axe_z = 0.0;
	pos_struct.roll_angle = 0.0;
	pos_struct.pitch_angle = 0.0;
	pos_struct.yaw_angle = 0.0;
	//Init de la structure intermédiaire
	int_struct.roll_axe_x = 0.0;
	int_struct.roll_axe_y = 0.0;
	int_struct.roll_axe_z = 0.0;
	int_struct.pitch_axe_x = 0.0;
	int_struct.pitch_axe_y = 0.0;
	int_struct.pitch_axe_z = 0.0;
	int_struct.yaw_axe_x = 0.0;
	int_struct.yaw_axe_y = 0.0;
	int_struct.yaw_axe_z = 0.0;
	distance_sensor_1  = 0.0;
	distance_sensor_2  = 0.0;
	distance_sensor_3  = 0.0;
	new_mode = 0;
	modify_mode = FALSE;
}

/**
* @brief Gère le calcul des distances selon les axes de roll,pitch et yaw.
* @pre   A executer dans le main avant la while(1)
*/
void PC_compute_position_mode_management(void) //Vrmt besoin d'une machine à état --> ça ralentit pas trop le programme ?
{
	typedef enum
	{
		OVERALL_INIT,
		COMPUTE_DISTANCE,
	}management_e;
	static management_e management_mode = OVERALL_INIT;
	switch(management_mode)
	{
	case OVERALL_INIT :
		PC_init();
		management_mode = COMPUTE_DISTANCE;
		break;
	case COMPUTE_DISTANCE :
		state = COMPUTING_ROLL_DIST;
		PC_compute_situation_from_angle();
		PC_compute_position();
		state = COMPUTING_PITCH_DIST;
		PC_compute_situation_from_angle();
		PC_compute_position();
		state = COMPUTING_YAW_DIST;
		PC_compute_situation_from_angle();
		PC_compute_position();
		state = COMPUTING_FINAL_POS;
		PC_compute_average();
		break;
	default :
		printf("Error managing position computing");
		break;
	}
}

/**
* @brief   Calcule la position du drone dans la cabine
* @info    A executer en tâche de fond
*/
void PC_compute_position(void)
{
	static situation_e computing_mode = COMPUTING_INIT;
	double angle_distance = 0.0;
	double gap = 0.0;
	if (modify_mode)
	{
		computing_mode = new_mode;
		modify_mode = FALSE;
	}
	switch(computing_mode)
	{
	case COMPUTING_INIT :
		break;
	case SAME_FIRST_SITUATION :
		if(state == COMPUTING_ROLL_DIST)
		{
			//AXE X
			angle_distance = sqrt((pow(CENTER_DISTANCE_X,2) + pow((pos_struct.roll_angle*0.016),2)));		//Rac(a^2+b^2) --> dist entre le mur et le capteur selon son angle
			gap = (distance_sensor_3 - angle_distance);
			int_struct.roll_axe_x = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.roll_angle)):0;
			//AXE Y
			angle_distance = sqrt((pow(CENTER_DISTANCE_Y,2) + pow((pos_struct.roll_angle*0.016),2)));
			gap = (distance_sensor_2 - angle_distance);
			int_struct.roll_axe_y = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(90)):0;
			//AXE Z
			angle_distance = sqrt((pow(CENTER_DISTANCE_Z,2) + pow((pos_struct.roll_angle*0.016),2)));
			gap = (distance_sensor_1 - angle_distance);
			int_struct.roll_axe_z = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.roll_angle)):0;
		}
		if(state == COMPUTING_PITCH_DIST)
		{
			//AXE X
			angle_distance = sqrt((pow(CENTER_DISTANCE_X,2) + pow((pos_struct.pitch_angle*0.016),2)));
			gap = (distance_sensor_3 - angle_distance);
			int_struct.pitch_axe_x = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.pitch_angle)):0;
			//AXE Y
			angle_distance = sqrt((pow(CENTER_DISTANCE_Y,2) + pow((pos_struct.pitch_angle*0.016),2)));
			gap = (distance_sensor_2 - angle_distance);
			int_struct.pitch_axe_y = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.pitch_angle)):0;
			//AXE Z
			angle_distance = sqrt((pow(CENTER_DISTANCE_Z,2) + pow((pos_struct.pitch_angle*0.016),2)));
			gap = (distance_sensor_1 - angle_distance);
			int_struct.pitch_axe_z = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.pitch_angle)):0;
		}
		if(state == COMPUTING_YAW_DIST)
		{
			//AXE X
			angle_distance = sqrt((pow(CENTER_DISTANCE_X,2) + pow((pos_struct.yaw_angle*0.016),2)));
			gap = (distance_sensor_3 - angle_distance);
			int_struct.yaw_axe_x = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.yaw_angle)):0;
			//AXE Y
			angle_distance = sqrt((pow(CENTER_DISTANCE_Y,2) + pow((pos_struct.yaw_angle*0.016),2)));
			gap = (distance_sensor_2 - angle_distance);
			int_struct.yaw_axe_y = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.yaw_angle)):0;
			//AXE Z
			angle_distance = sqrt((pow(CENTER_DISTANCE_Z,2) + pow((pos_struct.yaw_angle*0.016),2)));
			gap = (distance_sensor_1 - angle_distance);
			int_struct.yaw_axe_z = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.yaw_angle)):0;
		}
		break;
	case SECOND_SITUATION :
		if(state == COMPUTING_ROLL_DIST)
		{
			//CAPTEUR 1 EN FACE DE LA FACE BOTTOM OU TOP
			//ROLL AXE X
			angle_distance = sqrt((pow(CENTER_DISTANCE_X,2) + pow((pos_struct.roll_angle*0.016),2)));
			gap = (distance_sensor_3 - angle_distance);
			int_struct.roll_axe_x = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.roll_angle)):0;
			//ROLL AXE Y
			angle_distance = sqrt((pow(CENTER_DISTANCE_Z,2) + pow((pos_struct.roll_angle*0.016),2)));
			gap = (distance_sensor_1 - angle_distance);
			int_struct.roll_axe_y = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.roll_angle)):0;
			//ROLL AXE Z
			angle_distance = sqrt((pow(CENTER_DISTANCE_Y,2) + pow((pos_struct.roll_angle*0.016),2)));
			gap = (distance_sensor_2 - angle_distance);
			int_struct.roll_axe_z = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.roll_angle)):0;
		}
		if(state == COMPUTING_PITCH_DIST)
		{
			//CAPTEUR 1 EN FACE DE LA FACE BACKWARD OU FORWARD
			//PITCH AXE X
			angle_distance = sqrt((pow(CENTER_DISTANCE_X,2) + pow((pos_struct.pitch_angle*0.016),2)));
			gap = (distance_sensor_1 - angle_distance);
			int_struct.pitch_axe_x = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.pitch_angle)):0;
			//PITCH AXE Y
			angle_distance = sqrt((pow(CENTER_DISTANCE_Y,2) + pow((pos_struct.pitch_angle*0.016),2)));
			gap = (distance_sensor_2 - angle_distance);
			int_struct.pitch_axe_y = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.pitch_angle)):0;
			//PITCH AXE Z
			angle_distance = sqrt((pow(CENTER_DISTANCE_Z,2) + pow((pos_struct.pitch_angle*0.016),2)));
			gap = (distance_sensor_3 - angle_distance);
			int_struct.pitch_axe_z = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.pitch_angle)):0;
		}
		if(state == COMPUTING_YAW_DIST)
		{
			//CAPTEUR 2 EN FACE DE LA FACE BACKWARD OU FORWARD
			//YAW AXE X
			angle_distance = sqrt((pow(CENTER_DISTANCE_Y,2) + pow((pos_struct.yaw_angle*0.016),2)));
			gap = (distance_sensor_2 - angle_distance);
			int_struct.yaw_axe_x = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.yaw_angle)):0;
			//YAW AXE Y
			angle_distance = sqrt((pow(CENTER_DISTANCE_X,2) + pow((pos_struct.yaw_angle*0.016),2)));
			gap = (distance_sensor_3 - angle_distance);
			int_struct.yaw_axe_y = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.yaw_angle)):0;
			//YAW AXE Z
			angle_distance = sqrt((pow(CENTER_DISTANCE_Z,2) + pow((pos_struct.yaw_angle*0.016),2)));
			gap = (distance_sensor_1 - angle_distance);
			int_struct.yaw_axe_z = (fabs(gap) > fabs(PRECISION)) ? (gap*cos(pos_struct.yaw_angle)):0;
		}
		break;
	default :
		printf("Error computing position ");
		break;
	}

}

/**
* @brief Détermine la face (LEFT,RIGHT,UP,DOWN) devant laquelle se les capteurs du drone selon son angle (le capteur 1 est l'avant du drone)
*/
void PC_compute_situation_from_angle(void)
{
	//MODIF LE COMPUTING_MODE INDIRECTEMENT VIA LE NEW_MODE (PLUS SECU)
	switch(state)
	{
	case COMPUTING_ROLL_DIST :
		//ROLL ANGLE
		//Capteur 1 en face de LEFT ou RIGHT --> Capt1 mesure l'axe Z / Capt2 mesure axe Y / Capt3 mesure X
		// 0<angle<45 OU angle>315
		if(((pos_struct.roll_angle>=0 && pos_struct.roll_angle<=45)||(pos_struct.roll_angle>315))||((pos_struct.roll_angle>=135 && pos_struct.roll_angle<=225)))
		{
			new_mode = SAME_FIRST_SITUATION;
			modify_mode = TRUE;
		}
		//Capteur 1 en face de TOP ou BOTTOM --> Capt1 mesure l'axe Y / Capt2 mesure axe Z / Capt3 mesure axe X
		else
		{
			new_mode = SECOND_SITUATION;
			modify_mode = TRUE;
		}
		break;
	case COMPUTING_PITCH_DIST :
		//PITCH ANGLE
		//Capteur 1 en face de la face LEFT ou RIGHT --> Capt1 mesure l'axe Z / Capt2 mesure axe Y / Capt3 mesure X
		if(((pos_struct.pitch_angle>=0 && pos_struct.pitch_angle<=45)||(pos_struct.pitch_angle>315))||((pos_struct.pitch_angle>=135 && pos_struct.pitch_angle<=225)))
		{
			new_mode = SAME_FIRST_SITUATION;
			modify_mode = TRUE;
		}
		//Capteur 1 en face de BACKWARD ou FORWARD --> Capt1 mesure l'axe X / Capt2 mesure axe Y / Capt3 mesure axe Z
		else
		{
			new_mode = SECOND_SITUATION;
			modify_mode = TRUE;
		}
		break;
	case COMPUTING_YAW_DIST :
		//YAW ANGLE
		//Capteur 1 tjrs en face de LEFT, donc capteur 2 en face de TOP ou BOTTOM --> Capt1 mesure l'axe Z / Capt2 mesure axe Y / Capt3 mesure X
		if(((pos_struct.yaw_angle>=0 && pos_struct.yaw_angle<=45)||(pos_struct.yaw_angle>315))||((pos_struct.yaw_angle>=135 && pos_struct.yaw_angle<=225)))
		{
			new_mode = SAME_FIRST_SITUATION;
			modify_mode = TRUE;
		}
		//Capteur 2 en face de BACKWARD ou FORWARD --> Capt1 mesure axe Z / Capt2 mesure axe X / Capt3 mesure axe Y
		else
		{
			new_mode = SECOND_SITUATION;
			modify_mode = TRUE;
		}
		break;
	default :
		printf("Error computing situation from angle");
		break;
	}

}

/**
* @brief Reprend la structure intermédiaire pour faire une moyenne des dist calculées selon les diff axes et les stocker ds la structure finale de position
*/
void PC_compute_average(void) //Attention avec la division et les nbr decimaux ??
{
	double average = 0.0;
	average = (int_struct.roll_axe_x + int_struct.pitch_axe_x + int_struct.yaw_axe_x)/3;
	pos_struct.axe_x = average;
	average = (int_struct.roll_axe_y + int_struct.pitch_axe_y + int_struct.yaw_axe_y)/3;
	pos_struct.axe_y = average;
	average = (int_struct.roll_axe_z + int_struct.pitch_axe_z + int_struct.yaw_axe_z)/3;
	pos_struct.axe_z = average;
}

/**
* @brief Détermine les angles de roll,pitch,yaw et les stocke dans la structure finale de position
* @pre   Le MPU6050 doit être initialisé
*/
void PC_compute_angles(void)
{
	state = COMPUTING_ANGLES;
	//TEST
	pos_struct.roll_angle = 40;
	pos_struct.pitch_angle = 0;
	pos_struct.yaw_angle = 0;

}

/**
* @brief  Associe à l'aide de la librairie les distances respectives des capteurs TOF en fonction de leur position sur le drone (sensor_distance_1 --> Distance du capteur 1)
* @info   Utiliser la librairie pour calculer dist en fonction du num du capteur (dans les tab)
*/
void PC_compute_distances(void)
{
	state = COMPUTING_DISTANCES;
	//TEST
	distance_sensor_1 = 0.85;
	distance_sensor_2 = 0.92;
	distance_sensor_3 = 0.85;
}

/*void PC_get_position(void)
{
	double pos[5];
	pos[0] = pos_struct.axe_x;
	pos[1] = pos_struct.axe_y;
	pos[2] = pos_struct.axe_z;
	pos[3] = pos_struct.roll_angle;
	pos[4] = pos_struct.pitch_angle;
	pos[5] = pos_struct.yaw_angle;
	return pos[];

}*/
double PC_get_x(void)
{
	return pos_struct.axe_x;
}

double PC_get_y(void)
{
	return pos_struct.axe_y;
}

double PC_get_z(void)
{
	return pos_struct.axe_z;
}

double PC_get_roll(void)
{
	return pos_struct.roll_angle;
}

double PC_get_pitch(void)
{
	return pos_struct.pitch_angle;
}

double PC_get_yaw(void)
{
	return pos_struct.yaw_angle;
}








