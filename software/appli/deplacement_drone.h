/*
 * deplacement_drone.h
 *
 *  Created on: 30 juin 2020
 *      Author: Arnaud
 */

#ifndef APPLI_DEPLACEMENT_DRONE_H_
#define APPLI_DEPLACEMENT_DRONE_H_

#include "datas_process.h"

#define MINOR_ANGLE		5
//#define MAXIMAL_POWER

typedef enum
{
	FIXATION_INIT,
	FIXING,
	FIXED,
}fixes_e;

typedef enum
{
	NEED_FIXATION = 0,
	FIXATION_IN_PROGRESS,
	FIXATION_OK,
	FIXATION_TIMEOUT, //Au delà de 3 sec on recalcule ?
};

typedef enum{
	TRANS_Z_HAUT = 0,
	TRANS_Z_BAS
}trans_z_e;

<<<<<<< Updated upstream
=======
void DD_mode_management(void);
>>>>>>> Stashed changes


#endif /* APPLI_DEPLACEMENT_DRONE_H_ */
