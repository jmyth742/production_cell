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
#include <buttons.h>
#include <interface.h>
#include <control.h>
#include <can.h>
#include "messages.h"
#include "canbuffer.h"

/*************************************************************************
*                  PRIORITIES
*************************************************************************/

enum {
  APP_TASK_CAN_PRIO = 4,
  APP_TASK_CTRL_PRIO,
  APP_TASK_ITEMREADY_PRIO,
  APP_TASK_CHECKPICKUP_PRIO,
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

static OS_STK appTaskCanStk[APP_TASK_MONITOR_SENS_STK_SIZE];
//static OS_STK appTaskCtrlStk[APP_TASK_CTRL_STK_SIZE];

static OS_STK appTaskItemReadyStk[STK_SIZE];
static OS_STK appTaskCheckOutputStk[STK_SIZE];
static OS_STK appTaskCheckPickupStk[STK_SIZE];
static OS_STK appTaskDummyStk[STK_SIZE];

/*************************************************************************
*                  APPLICATION FUNCTION PROTOTYPES
*************************************************************************/

static void appTaskCan(void *pdata);
//static void appTaskCtrl(void *pdata);

static void appTaskItemReady(void *pdata);
static void appTaskCheckPickup(void *pdata);
static void appTaskCheckOutput(void *pdata);
static void appTaskDummy(void *pdata);
/*************************************************************************
*                    GLOBAL FUNCTION DEFINITIONS
*************************************************************************/

bool isRunning, isStopped, isPaused, error;

int main() {
  /* Initialise the hardware */
  bspInit();
  canInit();
  controlInit();

  /* Initialise the OS */
  OSInit();                                                   

  /* Create Tasks */
  OSTaskCreate(appTaskCan,                               
               (void *)0,
               (OS_STK *)&appTaskCanStk[STK_SIZE - 1],
               APP_TASK_CAN_PRIO);
   
//  OSTaskCreate(appTaskCtrl,                               
//               (void *)0,
//               (OS_STK *)&appTaskCtrlStk[APP_TASK_CTRL_STK_SIZE - 1],
//               APP_TASK_CTRL_PRIO);
    OSTaskCreate(appTaskItemReady,                               
               (void *)0,
               (OS_STK *)&appTaskItemReadyStk[STK_SIZE - 1],
               APP_TASK_ITEMREADY_PRIO);
    OSTaskCreate(appTaskCheckPickup,                               
               (void *)0,
               (OS_STK *)&appTaskCheckOutputStk[STK_SIZE - 1],
               APP_TASK_CHECKPICKUP_PRIO);
    OSTaskCreate(appTaskCheckOutput,                               
               (void *)0,
               (OS_STK *)&appTaskCheckPickupStk[STK_SIZE - 1],
               APP_TASK_CHECKOUTPUT_PRIO);
    OSTaskCreate(appTaskDummy,                               
               (void *)0,
               (OS_STK *)&appTaskDummyStk[STK_SIZE - 1],
               APP_TASK_DUMMY_PRIO);
   
  /* Start the OS */
  OSStart();                                                  
  
  /* Should never arrive here */ 
  return 0;      
}

/*************************************************************************
*                   APPLICATION TASK DEFINITIONS
*************************************************************************/
/*
 *   grabs messages from the CAN bus an buffers them.
 */
static void appTaskCan(void *pdata)
{
  osStartTick();
  static canMessage_t m;
  
  while(1){
    if(canReady(CAN_PORT_1)){
      canRead(CAN_PORT_1, &m);
      if(m.id == CONTROLLER_ID){
        
      }
    }  
    OSTimeDly(5);
  }
}
/*
 *  Sends input_pad_loaded message if pad 1 sensor active.
 */
static void appTaskItemReady(void *pdata)
{
  
  static canMessage_t m = {INPUT_ROBOT_ID, 2, INPUTPAD_LOADED, 0 };
  
  while(1)
  {
    ledToggle(USB_LINK_LED);
     if (controlItemPresent(CONTROL_SENSOR_1)) {
        interfaceLedSetState(D4_LED, LED_ON);
        //send INPUT_PAD_LOADED on CAN
        canWrite(CAN_PORT_1, &m);     
    } 
     else {
        interfaceLedSetState(D4_LED, LED_OFF);
     }
    OSTimeDly(500);
  }
}
/*
 *   Waits for robot 1 to attempt a pickup. Checks if block was removed.
 *   Sends pickup_ok on success of pickup_failed on failure.
 */
static void appTaskCheckPickup(void *pdata)
{
  char state = 1;
  static canMessage_t m;
  static canMessage_t ok   ={INPUT_ROBOT_ID, 2, PICKUP_OK, 0};
  static canMessage_t fail ={INPUT_ROBOT_ID, 2, PICKUP_FAILED, 0};
  static int attempts = 0;
  while(1)
  {
    //<-- pickup_attempted
    canRead(CAN_PORT_1, &m);
    if((m.id == CONTROLLER_ID) && (m.dataA == PICKUP_ATTEMPTED)){
      //check output pad clear
      if (controlItemPresent(CONTROL_SENSOR_1)) {
        //pickup failed
        canWrite(CAN_PORT_1, &fail);
        attempts++;
        interfaceLedSetState(D3_LED, LED_ON);
        if(attempts > 3){
          //non recoverable error
          //todo --- stop the thing ----------------------------------- !!!
          attempts = 0;
        }   
      }
      else {
        //pad is clear
        canWrite(CAN_PORT_1, &ok);
        attempts = 0;
        interfaceLedSetState(D3_LED, LED_OFF);
        //--> ack_remove
      }  
    }

    state = !state;
    interfaceLedSetState(D2_LED, (ledState_t)state);
    OSTimeDly(500);
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

//static void appTaskMonitorSens(void *pdata) {
//    
//  /* Start the OS ticker
//   * (must be done in the highest priority task)
//   */
//  osStartTick();
//  
//  /* 
//   * Now execute the main task loop for this task
//   */
//  while (true) {
//    interfaceLedSetState(D1_LED | D2_LED, LED_OFF);
//    ledSetState(USB_LINK_LED, LED_OFF);
//    ledSetState(USB_CONNECT_LED, LED_OFF);
//    
//    if (controlItemPresent(CONTROL_SENSOR_1)) {
//        interfaceLedSetState(D1_LED, LED_ON);
//        ledSetState(USB_LINK_LED, LED_ON);
//    } 
//    if (controlItemPresent(CONTROL_SENSOR_2)) {
//        interfaceLedSetState(D2_LED, LED_ON);
//        ledSetState(USB_CONNECT_LED, LED_ON);
//    } 
//    
//    OSTimeDly(20);
//  }
//}
//
//static void appTaskCtrl(void *pdata) {
//  static bool emergency = false;
//  interfaceLedSetState(D3_LED | D4_LED, LED_OFF);
//  
//  while (true) {
//    emergency = controlEmergencyStopButtonPressed();
//    if (emergency) {
//      controlAlarmToggleState();
//      interfaceLedSetState(D4_LED, LED_ON);
//      while (controlEmergencyStopButtonPressed()) {
//        OSTimeDly(20);
//      }
//    } else {
//      interfaceLedSetState(D4_LED, LED_OFF);
//    }
//    OSTimeDly(20);
//  } 
//}

