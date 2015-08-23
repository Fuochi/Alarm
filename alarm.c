/*
 Wireless Sensor Networks Laboratory

 Technische Universität München
 Lehrstuhl für Kommunikationsnetze
 http://www.lkn.ei.tum.de

 ---MUSEUM ALARM SYSTEM---

 Authors:
 Léo Dumas
 Lukas Obkircher
 Filippo Fuochi

 */

// Contiki-specific includes:
#include "core/contiki.h"
#include "core/dev/leds.h"            // Use LEDs.
#include "core/sys/clock.h"           // Use CLOCK_SECOND.
#include "dev/cc2420/cc2420.h"        // For cc2420_set_channel().
#include "core/sys/node-id.h"         // Manipulate the node_id.
#include "core/net/rime/rime.h"       // Establish connections.
#include "core/net/rime/broadcast.h"  // Establish connections.
#include "core/net/linkaddr.h"
#include "core/net/packetbuf.h"

// Standard C includes:
#include <stdio.h>                    // For printf.
#include <stdlib.h>                   // Abs()
#include "stdint.h"
#include "distanceSensors.h"
#include "defines.h"

#define UNICAST_RIME_CHANNEL 144 /*144 = Unicast + Ack, 146 = Unicast*/
#define BROADCAST_RIME_CHANNEL 129

/* --- Helper functions definitions --- */

static void check_for_invalid_addr(void);
static void print_settings(void);
static linkaddr_t generateLinkAddress(uint8_t nodeId);
#ifndef ALARM_MOTE
static void savePath(uint8_t hops, uint8_t sum, uint8_t * path);
#else
static uint8_t should_i_forward(uint8_t * path);
#endif

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from);
static void unicast_recv(struct unicast_conn *c, const linkaddr_t *from);

//Variables
static linkaddr_t linkAddr;
static struct broadcast_conn broadcast;
static struct unicast_conn unicast;
static struct etimer et;
#ifndef ALARM_MOTE
static uint8_t searchMote = 1;
static uint8_t searchMote_Index = 0;
static volatile uint8_t flooding_rx_flag = 0;
static volatile uint8_t uni_rx_flag = 0;
static uint8_t unicast_tryCounter = 0;
static uint8_t routingTable[ALARM_MOTE_COUNT][ROUTINGTABLE_SECOND_DIMENSION]; //Sum + Found + Hops + Path
#endif

// Callback-Functions
static const struct broadcast_callbacks broadcast_call = { broadcast_recv };
static const struct unicast_callbacks unicast_call = { unicast_recv };

typedef struct package {
    uint8_t source_id;
    uint8_t dest_id;
    uint8_t type;
    uint8_t data[ALARM_MOTE_COUNT + 1];
} Package;

static Package package = { 0, 0, 0, { 0 } };
static uint8_t msg_id_array[ALARM_MOTE_COUNT + 2] = { 0 }; /* Zero not used (MOTE-ID as Index) */

//------------------------ FUNCTIONS ------------------------
// Defines the behavior of a connection upon receiving data.
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {
    leds_on(LEDS_GREEN);
    if (((uint8_t*) packetbuf_dataptr())[SOURCE_ID_INDEX] != MOTE_ID) {
        //Forward only if I'm not the sender
        printf("Broadcast from %d [RSSI %d]:\n", from->u8[0], packetbuf_attr(PACKETBUF_ATTR_RSSI));

#ifdef ALARM_MOTE
        // React, depending on the Package-Type
        switch (*((uint8_t*) packetbuf_dataptr() + PACKAGE_TYPE_INDEX)) {
        case FLOODING_SEARCH:
            ; // <--- Otherwise compiler error
              //Check if MSG_ID is newer than previous one (greater or overflow)
            uint8_t rx_search_msg_id = ((uint8_t*) packetbuf_dataptr())[MSG_ID_INDEX];
            uint8_t rx_search_mote_id = ((uint8_t*) packetbuf_dataptr())[SOURCE_ID_INDEX];
            if (rx_search_msg_id > msg_id_array[rx_search_mote_id]
                    || (abs(((int16_t) msg_id_array[rx_search_mote_id]) - rx_search_msg_id) > 10)) {
                msg_id_array[rx_search_mote_id] = rx_search_msg_id;

                //Check if I'm the destination
                if (((uint8_t*) packetbuf_dataptr())[DEST_ID_INDEX] == MOTE_ID) {
                    // If I'm the destination --> Send FAck-Back to the central
                    // Fill Ack-Package
                    package.source_id = linkaddr_node_addr.u8[0];
                    package.dest_id = CENTRAL_MOTE_ID;
                    package.type = FLOODING_ACK;
                    package.data[0] = 0;					// SUM
                    int i;
                    for (i = 0; i < MAX_ALLOWED_HOPS; i++) {
                        package.data[i + 1] = 0;
                    }
                    // Send ACK
                    packetbuf_copyfrom(&package, PATH_INCL_PACKAGE_LEN);
                    broadcast_send(&broadcast);
                } else {
                    // I'm not the destination ---> forward package without any changes
                    broadcast_send(&broadcast);
                    printf("Fordward FLOODING SEARCH received from %d\n", from->u8[0]);

                }
            }
            break;
        case FLOODING_ALARM:
            ; //Check if MSG_ID is newer than previous one (greater or overflow)
            uint8_t rx_msg_id = ((uint8_t*) packetbuf_dataptr())[MSG_ID_INDEX];
            uint8_t rx_mote_id = ((uint8_t*) packetbuf_dataptr())[SOURCE_ID_INDEX];
            printf("Alarm msg id %d, saved msg_id %d\n", rx_msg_id, msg_id_array[rx_mote_id]);
            if (rx_msg_id > msg_id_array[rx_mote_id] || (abs(((int16_t) msg_id_array[rx_mote_id]) - rx_msg_id) > 10)) {
                // Save the newer MSG-ID, Let the LEDs blink and forward the Alarm (controlled Flooding)
                leds_on(LEDS_BLUE);
                leds_on(LEDS_RED);
                msg_id_array[rx_mote_id] = rx_msg_id;
                broadcast_send(&broadcast);
                leds_off(LEDS_BLUE);
                leds_off(LEDS_RED);
            }
            break;
        case FLOODING_ACK:
            // First check if I have already forwarded this package
            // If not --> add myself at the packages-tail, update RSSI-SUM and forward it
            // (RSSI-SUM is the sum of the RSSI over a defines threshold)
            // (Example: RSSI-SUM = 10, Threshold = 70: RSSI = 68 --> SUM = 10 + 0
            //                                          RSSI = 75 --> SUM = 10 + 5
            if (should_i_forward((uint8_t*) (packetbuf_dataptr() + PATH_INDEX))) {
                int8_t rssi_over_threshold = (
                        abs(packetbuf_attr(PACKETBUF_ATTR_RSSI)) > RSSI_THRESHOLD ?
                                abs(packetbuf_attr(PACKETBUF_ATTR_RSSI)) - RSSI_THRESHOLD : 0);
                ((uint8_t*) packetbuf_dataptr())[FLOODING_SUM_INDEX] += rssi_over_threshold;
                // No copying needed...package is already in the buffer
                broadcast_send(&broadcast);
            }
            break;
        default:
            printf("Unknown Package Type\n");
            break;
        }

#else //CENTRAL-MOTE
        // Print the Package-Data
        printf("Data: ");
        uint8_t i;
        for (i = 0; i < packetbuf_datalen(); i++) {
            printf("%d ,", ((uint8_t*) packetbuf_dataptr())[i]);
        }
        printf("\n");
        // React, depending on the Package-Type
        switch (*((uint8_t*) packetbuf_dataptr() + PACKAGE_TYPE_INDEX)) {
            case FLOODING_ALARM:
            // Alarm Received --> Send it to the QT-Interface
            printf("QT_ALARM %d %d %d\n", ((uint8_t*) packetbuf_dataptr())[SOURCE_ID_INDEX],
                    *((uint8_t*) packetbuf_dataptr() + FLOODING_DISTANCE_H_INDEX),
                    *((uint8_t*) packetbuf_dataptr() + FLOODING_DISTANCE_L_INDEX));
            break;
            case FLOODING_ACK:
            // Check if the received path has to be saved (do we have a path?, better rssi, ...)
            printf("ACK received!\n");
            if (searchMote == *((uint8_t*) packetbuf_dataptr() + SOURCE_ID_INDEX)) {
                uint8_t rx_hops = 0;
                uint8_t rx_sum = *((uint8_t*) packetbuf_dataptr() + FLOODING_SUM_INDEX);
                int8_t rssi_over_threshold = (
                        abs(packetbuf_attr(PACKETBUF_ATTR_RSSI)) > RSSI_THRESHOLD ?
                        (abs(packetbuf_attr(PACKETBUF_ATTR_RSSI))) - RSSI_THRESHOLD : 0);
                rx_sum += rssi_over_threshold;
                uint8_t i;
                for (i = 0; i < MAX_ALLOWED_HOPS; i++) {
                    if (*((uint8_t*) packetbuf_dataptr() + PATH_INDEX + i) == 0) {
                        break;
                    } else {
                        ++rx_hops;
                    }
                }
                // Three different Conditions for saving Path:
                // No-Path saved
                // Lower RSSI-SUM
                // Same RSSI-SUM but fewer hops
                if (routingTable[searchMote_Index][ROUTING_FOUND_INDEX] == 0) { //Only if no path is saved
                    savePath(rx_hops, rx_sum, (uint8_t*) (packetbuf_dataptr() + PATH_INDEX));//Same for all 3 for a better overview
                } else if (rx_sum < routingTable[searchMote_Index][ROUTING_SUM_INDEX]) {
                    savePath(rx_hops, rx_sum, (uint8_t*) (packetbuf_dataptr() + PATH_INDEX));
                } else if (rx_sum == routingTable[searchMote_Index][ROUTING_SUM_INDEX]
                        && rx_hops < routingTable[searchMote_Index][ROUTING_HOPS_INDEX]) {
                    savePath(rx_hops, rx_sum, (uint8_t*) (packetbuf_dataptr() + PATH_INDEX));
                }
                //Set Flag, so the main routine knows that we reveived a msg from the searched mote
                flooding_rx_flag = 1;
            }
            break;
            default:
            printf("Unknown Package Type");
            break;
        }

#endif
    }
    leds_off(LEDS_GREEN);
}

// Defines the behavior of a connection upon receiving data.
static void unicast_recv(struct unicast_conn *c, const linkaddr_t *from) {
    leds_on(LEDS_RED);

#ifdef ALARM_MOTE
    if (MOTE_ID != *((uint8_t*) packetbuf_dataptr() + DEST_ID_INDEX)) {
        // If we are not the destination
        // - Forward package to next node
        // - Decrease Hopcount
        uint8_t linkMoteNum;

        if (*((uint8_t*) packetbuf_dataptr() + UNI_HOPCOUNT_INDEX) > 1) {
            uint8_t index;
            // Path remains the same for both directions
            // Depending on direction we have to choose how to access the path
            // Central to Alarm_Mote   Path <---
            // Alarm_Mote to Central   ---> Path
            if (*((uint8_t*) packetbuf_dataptr() + DEST_ID_INDEX) != CENTRAL_MOTE_ID) {
                index = PATH_INDEX + (*((uint8_t*) packetbuf_dataptr() + UNI_HOPCOUNT_INDEX)) - 2;
            } else {
                index = packetbuf_datalen() - (*((uint8_t*) packetbuf_dataptr() + UNI_HOPCOUNT_INDEX)) + 1;
            }
            // Get Link from the calculated Index
            linkMoteNum = ((uint8_t*) packetbuf_dataptr())[index];
            //Decrease Hopcount
            (*((uint8_t*) packetbuf_dataptr() + UNI_HOPCOUNT_INDEX))--;
        } else {
            // Path-Empty: Get Link direct from the Destination
            linkMoteNum = *((uint8_t*) packetbuf_dataptr() + DEST_ID_INDEX);
        }
        // Generate Unicast-Link and forward the package
        linkAddr = generateLinkAddress(linkMoteNum);
        unicast_send(&unicast, &linkAddr);
        printf("Unicast Forward to %d\n", linkMoteNum);

    } else {
        switch (*((uint8_t*) packetbuf_dataptr() + PACKAGE_TYPE_INDEX)) {
        case UNI_CONFIG:
            //ToDo: prova
            break;
        case UNI_SEARCH:
            ;			// Modify the package inside the buffer and send UNI_ALIVE back
            uint8_t hopCount = packetbuf_datalen() - PATH_INDEX;
            (*((uint8_t*) packetbuf_dataptr() + SOURCE_ID_INDEX)) = MOTE_ID;
            (*((uint8_t*) packetbuf_dataptr() + DEST_ID_INDEX)) = CENTRAL_MOTE_ID;
            (*((uint8_t*) packetbuf_dataptr() + PACKAGE_TYPE_INDEX)) = UNI_ALIVE;
            (*((uint8_t*) packetbuf_dataptr() + UNI_HOPCOUNT_INDEX)) = hopCount;
            if (hopCount == 0) {
                linkAddr = generateLinkAddress(from->u8[0]);
            } else {
                linkAddr = generateLinkAddress((*((uint8_t*) packetbuf_dataptr() + PATH_INDEX)));
            }

            // The package is already in the buffer --> send it directly
            unicast_send(&unicast, &linkAddr);
            printf("Unicast Send UNI_ALIVE\n");
            break;
        default:
            printf("Unknown Package Type");
            break;
        }
    }
#else
    switch (*((uint8_t*) packetbuf_dataptr() + PACKAGE_TYPE_INDEX)) {
        case UNI_ALIVE:
        // Check if the msg is from the currently searched mote
        // If that is true --> set a flag for the main-loop and make the timer to expire
        if (searchMote == *((uint8_t*) packetbuf_dataptr() + SOURCE_ID_INDEX)) {
            uni_rx_flag = 1;
            //Make the Main-Loop-Timer to expire
            etimer_adjust(&et, -(etimer_expiration_time(&et) - 1));
        }
        break;
        default:
        printf("RX Unicast Package-Type: %d\n", *((uint8_t*) packetbuf_dataptr() + PACKAGE_TYPE_INDEX));
        printf("Unknown Package Type");
        break;
    }

#endif

    leds_off(LEDS_RED);
}

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS(example_broadcast_process, "Alarm");
AUTOSTART_PROCESSES(&example_broadcast_process);

//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD(example_broadcast_process, ev, data) {

    PROCESS_EXITHANDLER(broadcast_close(&broadcast)
    ;
    )
    PROCESS_BEGIN()
        ;
#ifdef ALARM_MOTE
        static uint16_t resend_interval = CLOCK_SECOND * 4;    // In seconds
        static uint16_t sensor_read_interval = CLOCK_SECOND / 8;
#else
        static uint16_t rxTimeout = CLOCK_SECOND * 2;
        static uint16_t check_alive_interval = CLOCK_SECOND;
#endif

        // Configure your team's channel (11 - 26).
        cc2420_set_channel(14);
        cc2420_set_txpower(15);

        // Set the node ID to generate a RIME address.
        node_id_burn(MOTE_ID);

        // Load the Node ID stored in flash memory.
        node_id_restore();

        print_settings();
        check_for_invalid_addr();
        packetbuf_clear();

        // Open broadcast connection.
        broadcast_open(&broadcast, BROADCAST_RIME_CHANNEL, &broadcast_call);
        unicast_open(&unicast, UNICAST_RIME_CHANNEL, &unicast_call);

#ifndef ALARM_MOTE
        // Clear Routing Table
        uint16_t nbytes = ALARM_MOTE_COUNT * (ALARM_MOTE_COUNT + 2);
        memset(&routingTable, 0, nbytes);
#else
        DistanceSensor_init(SENSOR_TYPE);
#endif

        while (1) {

#ifdef ALARM_MOTE
            // Check for Movement --> Alarm
            if (SENSOR_TYPE != NO_SENSOR && DistanceSensor_MovementDetected()) {
                // Alarm triggered! --> Calculate Distance, Generate Alarm-Package and Send it
                uint16_t distance_cm = DistanceSensor_getDistance_cm();
                leds_on(LEDS_BLUE);
                leds_on(LEDS_RED);
                package.dest_id = CENTRAL_MOTE_ID;
                package.source_id = MOTE_ID;
                package.type = FLOODING_ALARM;
                *((uint8_t*) &package + MSG_ID_INDEX) = ++msg_id_array[MOTE_ID];
                *((uint8_t*) &package + FLOODING_SENSOR_INDEX) = SENSOR_TYPE;
                *((uint8_t*) &package + FLOODING_DISTANCE_H_INDEX) = (uint8_t) (distance_cm >> 8);
                *((uint8_t*) &package + FLOODING_DISTANCE_L_INDEX) = (uint8_t) (distance_cm & 0xFF);
                packetbuf_copyfrom(&package, ALARM_PACKAGE_LEN);
                broadcast_send(&broadcast);
                printf("AlarmData send: %d, %d, %d, %d, %d\n", package.source_id, *((uint8_t*) &package + MSG_ID_INDEX),
                        *((uint8_t*) &package + FLOODING_SENSOR_INDEX),
                        *((uint8_t*) &package + FLOODING_DISTANCE_H_INDEX),
                        *((uint8_t*) &package + FLOODING_DISTANCE_L_INDEX));
                leds_off(LEDS_RED);
                leds_off(LEDS_BLUE);
                //Wait for a few seconds to prevent congestion
                //If then there is still a movement/close-detection --> fire it again!
                etimer_set(&et, resend_interval);
                PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            } else {
                // Normal Operation...simply wait
                etimer_set(&et, sensor_read_interval);
                PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            }

#else // CENTRAL-MOTE
            printf("Start Searching...\n");
            // First generate Routing-Table by using controlled flooding
            // Then periodically check if each mote is alive (with unicast)
            // If a mote is lost -> search it again with controlled flooding
            // Update the QT-Interface after each search
            for (searchMote = MIN_ALARM_MOTE_ID; searchMote <= MAX_ALARM_MOTE_ID; searchMote++) {
                searchMote_Index = searchMote - MIN_ALARM_MOTE_ID;

                // If we have a path --> Unicast for Check-Alive
                if (routingTable[searchMote_Index][ROUTING_FOUND_INDEX]) {
                    unicast_tryCounter = 0;
                    do {
                        printf("Search for %d with Unicast: ", searchMote);
                        package.source_id = CENTRAL_MOTE_ID;
                        package.dest_id = searchMote;
                        package.type = UNI_SEARCH;
                        ((uint8_t*) &package)[UNI_HOPCOUNT_INDEX] = routingTable[searchMote_Index][ROUTING_HOPS_INDEX];
                        uint8_t i;
                        for (i = 0; i < routingTable[searchMote_Index][ROUTING_HOPS_INDEX]; i++) {
                            ((uint8_t*) &package)[PATH_INDEX + i] = routingTable[searchMote_Index][ROUTING_PATH_INDEX
                            + i];
                        }
                        // Check for direct connection
                        if (routingTable[searchMote_Index][ROUTING_HOPS_INDEX] == 0) {
                            linkAddr = generateLinkAddress(searchMote);
                        } else {
                            linkAddr = generateLinkAddress(
                                    routingTable[searchMote_Index][ROUTING_PATH_INDEX
                                    + routingTable[searchMote_Index][ROUTING_HOPS_INDEX] - 1]);
                        }
                        packetbuf_copyfrom(&package, PATH_INDEX + routingTable[searchMote_Index][ROUTING_HOPS_INDEX]);
                        unicast_send(&unicast, &linkAddr);
                        uni_rx_flag = 0;

                        etimer_set(&et, rxTimeout);
                        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
                    }while (!uni_rx_flag && ++unicast_tryCounter < MAX_UNICAST_TRIES);
                    if (!uni_rx_flag) {
                        printf("FAILURE: Route to %d lost!\n", searchMote);
                        //Clear Routing-Table for this Mote
                        memset(routingTable[searchMote_Index], 0, ROUTINGTABLE_SECOND_DIMENSION);
                    } else {
                        printf(" Route is OK!!!\n");
                    }
                }

                // We don't have a path or path is lost --> Search mote with controlled Flooding
                if (!routingTable[searchMote_Index][ROUTING_FOUND_INDEX]) {
                    printf("Search for %d with Flooding: ", searchMote);
                    //Collect Package
                    package.dest_id = searchMote;
                    package.source_id = CENTRAL_MOTE_ID;
                    package.type = FLOODING_SEARCH;
                    ((uint8_t*) &package)[MSG_ID_INDEX] = ++msg_id_array[MOTE_ID];

                    //Send Broadcast
                    packetbuf_copyfrom(&package, FLOODING_SEARCH_LEN);
                    broadcast_send(&broadcast);

                    //Wait as long as no package arrives until rxTimeout
                    do {
                        flooding_rx_flag = 0;
                        etimer_stop(&et);
                        etimer_set(&et, rxTimeout);
                        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
                    }while (flooding_rx_flag);

                    // Debugging Output
                    if (routingTable[searchMote_Index][ROUTING_FOUND_INDEX]) {
                        printf("Path found: ");
                        if (routingTable[searchMote_Index][ROUTING_HOPS_INDEX]) {
                            // Print hops
                            uint8_t i;
                            printf(" --->");
                            for (i = 0; i < routingTable[searchMote_Index][ROUTING_HOPS_INDEX]; i++) {
                                printf(" %d ", routingTable[searchMote_Index][ROUTING_PATH_INDEX + i]);
                            }
                            printf("<--- ");
                        }
                        printf("Hops: %d, RSSI-Sum: %d\n", routingTable[searchMote_Index][ROUTING_HOPS_INDEX],
                                routingTable[searchMote_Index][ROUTING_SUM_INDEX]);
                    } else {
                        printf(" ### ERROR ### No Path found!\n");
                    }

                }
                //Forward the updated routing-table to the visual interface
                printf("QT_ROUTE %d %d %d ", searchMote, routingTable[searchMote_Index][ROUTING_FOUND_INDEX],
                        routingTable[searchMote_Index][ROUTING_SUM_INDEX]);
                uint8_t i;
                for (i = 0; i < routingTable[searchMote_Index][ROUTING_HOPS_INDEX]; i++) {
                    printf("->%d",
                            routingTable[searchMote_Index][ROUTING_PATH_INDEX
                            + routingTable[searchMote_Index][ROUTING_HOPS_INDEX] - i - 1]);
                }
                printf("\n");
                etimer_set(&et, check_alive_interval);
                PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            }
            printf("Search done!\n");
#endif

        }
        DistanceSensor_deInit();

    PROCESS_END()
;
}

/* --- Helper functions definitions --- */

// Checks if the loaded RIME address is all zeroes.
static void check_for_invalid_addr(void) {
// Boolean flag to check invalid address.
static int iAmError = 0;

// All-zeroes address.
static linkaddr_t errAddr;
errAddr.u8[0] = 0;
errAddr.u8[1] = 0;

// Check if this mote got an invalid address.
iAmError = linkaddr_cmp(&errAddr, &linkaddr_node_addr);

// Turn ON all LEDs if we loaded an invalid address.
if (iAmError) {
    printf("\nLoaded an invalid RIME address (%u.%u)! "
            "Reset the device.\n\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);

// Freezes the app here. Reset needed.
    while (1) {
        leds_on(LEDS_ALL);
    }
}
}

#ifdef ALARM_MOTE
/*
 * Checks if own ID in already inside the Path, or Path is too long
 * If not: add own ID to it's tail
 */
static uint8_t should_i_forward(uint8_t * path) {
uint8_t i;
uint8_t forward = 0;
for (i = 0; i < ALARM_MOTE_COUNT; i++) {
    if (path[i] == 0) {
// Add Own ID to path
        path[i] = MOTE_ID;
        forward = 1;
        break;
    } else if (path[i] == MOTE_ID || (i >= (MAX_ALLOWED_HOPS - 1))) {
        break;
    }
}
if (!forward) {
    printf("Not forwarding package from 0: %d\n", path[0]);
}
return forward;
}
#endif

#ifndef ALARM_MOTE

// Save the received path from the FAck-Package into the routing table
static void savePath(uint8_t hops, uint8_t sum, uint8_t * path) {
printf("saving path\n");
routingTable[searchMote_Index][ROUTING_HOPS_INDEX] = hops;
routingTable[searchMote_Index][ROUTING_SUM_INDEX] = sum;
routingTable[searchMote_Index][ROUTING_FOUND_INDEX] = 1;
uint8_t i = 0;
for (; hops > i; i++) {
    routingTable[searchMote_Index][ROUTING_PATH_INDEX + i] = path[i]; /*path[hops - i - 1] to change the direction*/
}
}

#endif

// Generate link address from node ID
static linkaddr_t generateLinkAddress(uint8_t nodeId) {
linkaddr_t addr;

addr.u8[0] = nodeId;
addr.u8[1] = 0;

return addr;
}

// Prints the current settings.
static void print_settings(void) {
printf("\n-----------------------------\n");
printf("Node ID = \t%u\n", node_id);
printf("RIME addr = \t%u.%u\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
printf("Using radio channel %d\n", cc2420_get_channel());
printf("-----------------------------\n");
}
