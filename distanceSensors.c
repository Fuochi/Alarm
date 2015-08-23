/*
 Wireless Sensor Networks Laboratory

 Technische Universität München
 Lehrstuhl für Kommunikationsnetze
 http://www.lkn.ei.tum.de

 Authors:
 Léo Dumas
 Lukas Obkircher
 Filippo Fuochi
 */

// Contiki-specific includes:
#include "core/contiki.h"
#include "core/dev/leds.h"   // Use LEDs.
#include "core/lib/sensors.h"                 //required for sensor_sensor struct that is used to define phidget struct
#include "platform/z1/dev/z1-phidgets.h" //definition of the phidget struct

// Standard C includes:
#include <stdio.h>                    // For printf.
#include "defines.h"
#include <math.h>

static uint8_t sensorType;

// Init Sensors depending on type
void DistanceSensor_init(uint8_t type) {
    sensorType = type;
    SENSORS_ACTIVATE(phidgets);
}

void DistanceSensor_deInit() {
    SENSORS_DEACTIVATE(phidgets);
}

// Get ADC-Sensor-Value !!!ATTENTION: Depends of battery or USB-powered!!!
uint16_t DistanceSensor_getValue() {
    uint16_t sensor_value = phidgets.value(PHIDGET5V_1);
    return sensor_value;
}

// Calculate the Distance depending on the sensors
uint16_t DistanceSensor_getDistance_cm() {

    uint16_t sensor_value = DistanceSensor_getValue();
    uint16_t distance;
    printf("Voltage %d\n", sensor_value);
    if (sensorType == IR) {
        // Approximation according to our measurement
        distance = (int) (23000.0f / sensor_value);
    } else {
        // Approximation according to the datasheet
        distance = sensor_value * 0.3272727f;
    }
    return distance;

}

// Movement detected if the Sensorvalues are greater/smaller (depending on the sensor) than the trigger values
uint8_t DistanceSensor_MovementDetected() {
    uint8_t iRet = 0;
    if ((sensorType == IR && DistanceSensor_getValue() > IR_TRIGGER_VALUE)
            || (sensorType == SONAR && DistanceSensor_getValue() < SONAR_TRIGGER_VALUE)) {
        iRet = 1;
    }
    return iRet;
}

