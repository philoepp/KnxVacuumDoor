#ifndef KNX_VACUUM_DOOR_H
#define KNX_VACUUM_DOOR_H

#include <Arduino.h>

/* -------------------------------------------------------------------------- 
* DEFINES
---------------------------------------------------------------------------- */
// Define the physical address of the KNX device
#define KNX_PA                      "1.0.182" // PA of the KNX device
#define SEND_INTERVAL_CURRENT_POSITION  (60 * 1000) // [ms] 60s send interval

// Group address "Outputs"
#define KNX_GA_DOOR_POSITION        "0/0/0"  // GA for the measured concrete temperature

// Group address "Inputs"
#define KNX_GA_DOOR_SETPOINT        "0/0/0"  // GA for the measured room temperature

/* -------------------------------------------------------------------------- 
* ENUMS
---------------------------------------------------------------------------- */

enum DoorState
{
  DoorClosed = 0,
  DoorOpened = 1
};

/* -------------------------------------------------------------------------- 
* STRUCTS/DATATYPES
---------------------------------------------------------------------------- */
struct VacuumDoor 
{
  boolean Setpoint;
  boolean CurrentPosition;
};

#endif //KNX_VACUUM_DOOR_H