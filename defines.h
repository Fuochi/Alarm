/*
 * defines.h
 *
 *  Created on: Jun 9, 2015
 *  Authors:
 *     Léo Dumas
 *     Lukas Obkircher
 *     Filippo Fuochi
 */

#ifndef DEFINES_H_
#define DEFINES_H_

enum SENSORS_TYPE {
    IR, SONAR, NO_SENSOR
};
enum SENSOR_STATUS {
    OFF, ON
};
enum PROTOCOLPACKAGE {
    SOURCE_ID_INDEX, DEST_ID_INDEX, PACKAGE_TYPE_INDEX, FIRST_DATA_INDEX
};

/* MAIN CONFIGURATION BEGIN */
//#define ALARM_MOTE /* COMMENT TO CHANGE TO CENTRAL-MOTE */

#ifdef ALARM_MOTE
#define MOTE_ID 3
#define SENSOR_TYPE SONAR
#else
#define MOTE_ID CENTRAL_MOTE_ID /* CENTRAL MOTE */
#endif

#define IR_TRIGGER_VALUE	900 /*Trigger if value is geater*/
#define SONAR_TRIGGER_VALUE 100 /*Trigger if value is smaller*/
/* MAIN CONFIGURATION END*/

#define MIN_ALARM_MOTE_ID 2
#define MAX_ALARM_MOTE_ID 7
#define ALARM_MOTE_COUNT (MAX_ALARM_MOTE_ID - MIN_ALARM_MOTE_ID + 1)
#define CENTRAL_MOTE_ID 1
#define RSSI_THRESHOLD 70
#define MAX_UNICAST_TRIES	3
#define MAX_ALLOWED_HOPS 3

//Package Length
#define ALARM_PACKAGE_LEN (7) /* Source, Dest, Type, MSG-ID, Sensor, Dist-H, Dist-L */
#define PATH_INCL_PACKAGE_LEN	(MAX_ALLOWED_HOPS + 4) /* Source, Dest, Type, FirstData */

//Types
#define UNI_SEARCH 0
#define UNI_ALIVE 1
#define UNI_CONFIG 2
#define FLOODING_ALARM	3
#define FLOODING_SEARCH	4
#define FLOODING_ACK	5

//PackageData
#define PATH_INDEX (FIRST_DATA_INDEX + 1) /* Same for both packages */
#define MSG_ID_INDEX (FIRST_DATA_INDEX) /* Same for both packages */
#define UNI_HOPCOUNT_INDEX FIRST_DATA_INDEX
#define UNI_STATE_INDEX FIRST_DATA_INDEX
#define UNI_RSSI_INDEX (FIRST_DATA_INDEX+1)
#define FLOODING_SENSOR_INDEX (FIRST_DATA_INDEX + 1)
#define FLOODING_DISTANCE_H_INDEX (FIRST_DATA_INDEX +2)
#define FLOODING_DISTANCE_L_INDEX (FIRST_DATA_INDEX + 3)
#define FLOODING_SUM_INDEX (FIRST_DATA_INDEX)

//Routing-Table
#define ROUTING_HOPS_INDEX	0
#define ROUTING_SUM_INDEX 	1
#define ROUTING_FOUND_INDEX	2
#define ROUTING_PATH_INDEX	3
#define ROUTINGTABLE_SECOND_DIMENSION (ALARM_MOTE_COUNT + 3) /*Sum + Found + Hops + Path */

//Package-Length
#define FLOODING_SEARCH_LEN 4

#endif /* DEFINES_H_ */

