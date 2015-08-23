/*
 * sensors.h
 *
 *  Created on: Jun 9, 2015
 *  Authors:
 *     Léo Dumas
 *     Lukas Obkircher
 *     Filippo Fuochi
 */

#ifndef DISTANCESENSORS_H_
#define DISTANCESENSORS_H_

void DistanceSensor_init(uint8_t type);
void DistanceSensor_deInit();
uint16_t DistanceSensor_getValue();
uint8_t DistanceSensor_MovementDetected();
uint16_t DistanceSensor_getDistance_cm();

#endif /* DISTANCESENSORS_H_ */
