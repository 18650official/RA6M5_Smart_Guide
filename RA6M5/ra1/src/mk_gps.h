/*
 * mk_gps.h
 *
 *  Created on: 2025年7月30日
 *      Author: Snowmiku
 */

#ifndef MK_GPS_H_
#define MK_GPS_H_

#include "hal_data.h"
#include "bsp_api.h"
#include "mk_pinctrl.h"
#include "minmea.h"
#include "string.h"
#include "stdio.h"

typedef struct
{
	float lat;
	float lon;
	bool isSucceed;
} GpsLocation_Typedef;

#endif /* MK_GPS_H_ */

bool parseGNRMCData(GpsLocation_Typedef* loc, uint8_t* line);


