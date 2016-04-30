/* Control Panel Test
 * Emergency stop -> toggle Alarm State
 *   Data LED 3 lit while emergency stop button pressed
 *
 * Pedestal sensor 1 reported  on interface LED 1 and LINK LED
 * Pedestal sensor 2 reported  on interface LED 2 and CONNECT LED
 */

#include <stdbool.h>
#include <ucos_ii.h>
#include <bsp.h>
#include <osutils.h>
#include <leds.h>
#include <lcd.h>
#include <buttons.h>
#include <interface.h>
#include <control.h>
#include <can.h>
#include "messages.h"
#include "canbuffer.h"
#include "controller2.h"

/*************************************************************************
*                  PRIORITIES
*************************************************************************/

enum {
  APP_TASK_CTRL_PRIO = 4,
  APP_TASK_CHECKPICKUP_PRIO,
  APP_TASK_ITEMREADY_PRIO,
  APP_TASK_CHECKOUTPUT_PRIO,
  APP_TASK_DUMMY_PRIO
};

/*************************************************************************
*                     APPLICATION TASK STACKS
*************************************************************************/

enum {
  APP_TASK_MONITOR_SENS_STK_SIZE = 256,
  APP_TASK_CTRL_STK_SIZE = 256,
  STK_SIZE = 256
};

static OS_STK appTaskCtrlStk[APP_TASK_CTRL_STK_SIZE];
static OS_STK appTaskItemReadyStk[STK_SIZE];
static OS_STK appTaskCheckOutputStk[STK_SIZE];
static OS_STK appTaskCheckPickupStk[STK_SIZE];
static OS_STK appTaskDummyStk[STK_SIZE];

/*************************************************************************
*                  APPLICATION FUNCTION PROTOTYPES
*************************************************************************/

static void appTaskCtrl(void *pdata);
static void appTaskItemReady(void *pdata);
static void appTaskCheckPickup(void *pdata);
static void appTaskCheckOutput(void *pdata);
static void appTaskDummy(void *pdata);
/*************************************************************************
*                    GLOBAL FUNCTION DEFINITIONS
*************************************************************************/
bool StartupChecks(void);

static bool isRunning, isStopped, isPaused, error;

int main() {
  /* Initialise the hardware */
  bspInit();
  canInit();
  controlInit();
  //canBufferInit();

  /* Initialise the OS */
  OSInit();                                                   

  /* Create Tasks */ 
    OSTaskCreate(appTaskCtrl,                               
               (void *)0,
               (OS_STK *)&appTaskCtrlStk[APP_TASK_CTRL_STK_SIZE - 1],
               APP_TASK_CTRL_PRIO); 
    OSTaskCreate(appTaskItemReady,                               
               (void *)0,
               (OS_STK *)&appTaskItemReadyStk[STK_SIZE - 1],
               APP_TASK_ITEMREADY_PRIO);
    OSTaskCreate(appTaskCheckPickup,                               
               (void *)0,
               (OS_STK *)&appTaskCheckOutputStk[STK_SIZE - 1],
               APP_TASK_CHECKPICKUP_PRIO);
//    OSTaskCreate(appTaskCheckOutput,                               
//               (void *)0,
//               (OS_STK *)&appTaskCheckPickupStk[STK_SIZE - 1],
//               APP_TASK_CHECKOUTPUT_PRIO);
//    OSTaskCreate(appTaskDummy,                               
//               (void *)0,
//               (OS_STK *)&appTaskDummyStk[STK_SIZE - 1],
//               APP_TASK_DUMMY_PRIO);
   
  //pre start checks
    if(StartupChecks()) {
      OSStart();  
    }     
    else {
    
    }
  
  /* Should never arrive here */ 
  return 0;      
}

/*************************************************************************
*                   APPLICATION TASK DEFINITIONS
*************************************************************************/

/*
 *  Sends input_pad_loaded message if pad 1 sensor active. ---------------------------------------------------------------------------------
 */
static void appTaskItemReady(void *pdata)
{
  static canMessage_t m = {INPUT_ROBOT_ID, 4, INPUTPAD_LOADED, 0};
  bool sent;
  while(1)
  {
     if (controlItemPresent(CONTROL_SENSOR_1)) {
        interfaceLedSetState(D4_LED, LED_ON);
        //send INPUT_PAD_LOADED on CAN
        if(!sent){
          canWrite(CAN_PORT_1, &m);  
          lcdSetTextPos(1,1);
          lcdWrite("ID: %i sent", m.id);      
          sent = true;
        }
    } 
     else {
        interfaceLedSetState(D4_LED, LED_OFF);
        //lcdSetTextPos(1,1);
        //lcdWrite("no item    ");
        sent = false;
     }
//     state = !state;
//    interfaceLedSetState(D1_LED, (ledState_t)state);
    OSTimeDly(800);
  }
}
/*
 *   Waits for robot 1 to attempt a pickup. Checks if block was removed.
 *   Sends pickup_ok on success of pickup_failed on failure.
 */
static void appTaskCheckPickup(void *pdata)
{
  //osStartTick();
  char state = 1;
  static canMessage_t m; 
  static canMessage_t ok = {INPUT_ROBOT_ID, 4, PICKUP_OK, 0};
  static canMessage_t fail = {INPUT_ROBOT_ID, 4, PICKUP_FAILED, 0};
  static int attempts = 0;
  while(1)
  {
//    //<-- pickup_attempted
    if(canReady(CAN_PORT_1)){
      canRead(CAN_PORT_1, &m);
      lcdSetTextPos(0,0);
      lcdWrite("id: %i m: %i", m.id, m.dataA);
      if((m.id == CONTROLLER_ID) && (m.dataA == PICKUP_ATTEMPTED)){
        //check output pad clear
        if (controlItemPresent(CONTROL_SENSOR_1)) {
          //pickup failed
          canWrite(CAN_PORT_1, &fail);
          lcdSetTextPos(1,6);
          lcdWrite("Sent pickupFail");
          
          attempts++;
          interfaceLedSetState(D3_LED, LED_ON);
          if(attempts > 3){
            lcdSetTextPos(1,8);
            lcdWrite("Robot 1 Fail");
            //non recoverable error
            //todo --- stop the thing ----------------------------------- !!!
            attempts = 0;
          }   
        }
        else {
          //pad is clear
          
          attempts = 0;
          interfaceLedSetState(D3_LED, LED_OFF);
          lcdSetTextPos(1,8);
          lcdWrite("Sent pickupOK");
          //--> ack_remove
          canWrite(CAN_PORT_1, &ok);
        }  
      }
    }
    state = !state;
    interfaceLedSetState(D2_LED, (ledState_t)state);
    OSTimeDly(100);
  }
}

/*
 *    Checks the output pad when a request to deposit from
 *    robot 2 is recieved. If pad is clear sends an ack.
 */
static void appTaskCheckOutput(void *pdata)
{
  char state = 0;
  static canMessage_t m;
  static canMessage_t ack = {OUTPUT_ROBOT_ID, 2, ACK_REMOVEBLOCK, 0};
  while(1)
  {
    if(canReady(CAN_PORT_1)){
      canRead(CAN_PORT_1, &m);
      if((m.id == CONTROLLER_ID)&&(m.dataA == REQ_REMOVEBLOCK)){
        //request recieved from robot 2
        if (controlItemPresent(CONTROL_SENSOR_2)){
          //output pad is clear, send ack.
          canWrite(CAN_PORT_1, &ack);  
        }
      }
      state = !state;
      interfaceLedSetState(D1_LED, (ledState_t)state);
      OSTimeDly(500);
    }
  }
}

static void appTaskDummy(void *pdata)
{
  char state = 0;
  while(1)
  {
    state = !state;
    //interfaceLedSetState(D1_LED, state);
    ledSetState(USB_CONNECT_LED, (ledState_t)state);
    OSTimeDly(500);
  }
}
//
//void readyLightToggle(void);
//void runningLightToggle(void);
//void pausedLightToggle(void);
//void emergencyLightToggle(void);
//bool startBtn(void);
//bool pauseBtn(void);
//bool errorBtn(void);
//bool emergencyBtn(void);
static void appTaskCtrl(void *pdata) {
  osStartTick();
  static bool emergency = false;
  interfaceLedSetState(D3_LED | D4_LED, LED_OFF);
  
  while (true) {
   emergency = controlEmergencyStopButtonPressed();
    if (emergency) {
      controlAlarmToggleState();
      //emergencyLightToggle();
      //while (emergency) {
//        OSTimeDly(20);
//      }
    } else {
      //emergencyLightToggle();
      //controlAlarmToggleState();
    }
    OSTimeDly(500);
  } 
}

/*
 *  Start the OS if startup checks OK
 */
bool StartupChecks()
{
  static canMessage_t m;
  m.id = CONVEYOR_ID;
  return true;
}