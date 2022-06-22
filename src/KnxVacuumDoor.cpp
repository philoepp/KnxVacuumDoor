#include <Arduino.h>
#include <KnxTpUart.h>
#include "KnxVacuumDoor.h"

/* -------------------------------------------------------------------------- 
* DEFINES
---------------------------------------------------------------------------- */
//#define DEBUG

/* -------------------------------------------------------------------------- 
Objects/variables
---------------------------------------------------------------------------- */
// KNX device
KnxTpUart knx(&Serial, KNX_PA);
VacuumDoor Door = {0};

/* -------------------------------------------------------------------------- 
* STATIC FUNCTION PROTOTYPES
---------------------------------------------------------------------------- */
static void vInitializeKNX(void);
static void vSendCurrentPositionToKnx(void);

/* -------------------------------------------------------------------------- 
* FUNCTIONS
---------------------------------------------------------------------------- */
void setup(void)
{
#ifdef DEBUG
  Serial.begin(9600);
#endif
  vInitializeKNX();
}

void loop(void)
{ 
  // Send calculated/measured values to the KNX bus
  vSendCurrentPositionToKnx();
}

/* -------------------------------------------------------------------------- 
* STATIC FUNCTIONS
---------------------------------------------------------------------------- */
static void vSendCurrentPositionToKnx(void)
{
  static uint32_t u32LastTime = millis();
  static bool fLastSendState = false;

  // Check if an update of the relay state is needed
  if(fLastSendState != Door.CurrentPosition)
  {
    fLastSendState = Door.CurrentPosition;
    knx.groupWriteBool(KNX_GA_DOOR_POSITION, Door.CurrentPosition);
  }

  // Additional cyclic send of current position
  if((millis() - u32LastTime) > SEND_INTERVAL_CURRENT_POSITION)
  {
    u32LastTime = millis(); 
    knx.groupWriteBool(KNX_GA_DOOR_POSITION, Door.CurrentPosition);
  }
}

static void vInitializeKNX(void) 
{
  // Initialize connection to KNX BCU
  Serial.begin(19200,SERIAL_8E1); // Even parity;
  while (!Serial) {
    ; // wait for serial port to connect
  }

  // Reset UART
  if(Serial.available()) {
    knx.uartReset();
  }

  // Register KNX group addresses
  knx.addListenGroupAddress(KNX_GA_DOOR_POSITION);
  knx.addListenGroupAddress(KNX_GA_DOOR_SETPOINT);
}

void serialEvent() 
{
  KnxTpUartSerialEventType eType = knx.serialEvent();

  if(eType == KNX_TELEGRAM) 
  {
    KnxTelegram* telegram = knx.getReceivedTelegram();

    String target = String(0 + telegram->getTargetMainGroup())   + "/" +
                    String(0 + telegram->getTargetMiddleGroup()) + "/" +
                    String(0 + telegram->getTargetSubGroup());

#ifdef DEBUG
  Serial.print("KNX Event on: ");
  Serial.println(target);
#endif

    // Is it a read request?
    switch(telegram->getCommand())
    {
      case KNX_COMMAND_READ:
        // Current enable/disable state
        if(strcmp(target.c_str(), KNX_GA_DOOR_POSITION) == 0) 
        {
          knx.groupAnswerBool(KNX_GA_DOOR_POSITION, Door.CurrentPosition);
        }
        // Current room temperature setpoint
        else if(strcmp(target.c_str(), KNX_GA_DOOR_SETPOINT) == 0) 
        {
          knx.groupAnswerBool(KNX_GA_DOOR_SETPOINT, Door.Setpoint);
        } 
        break;

      case KNX_COMMAND_WRITE:
        // Room temperature setpoint correction
        if(strcmp(target.c_str(), KNX_GA_DOOR_SETPOINT) == 0) 
        {
          Door.Setpoint = telegram->getBool();
#ifdef DEBUG
        Serial.print("New position setpoint: ");
        Serial.println(Door.Setpoint);
#endif
        } 
        break;

      default:
        break;
    }
  }
}
