/* Conveyor Test
 * JS UP -> forward
 *    DN -> reverse
 *    L,R -> stop
 * 
 * Conveyor sensor 1 reported  on interface LED 1 and LINK LED
 * Conveyor sensor 2 reported  on interface LED 2 and CONNECT LED
 *
 * item sensed at 1 and moving in reverse -> stop
 * item sensed at 2 and moving forward -> stop
 */

#include <stdbool.h>
#include <ucos_ii.h>
#include <bsp.h>
#include <osutils.h>
#include <leds.h>
#include <buttons.h>
#include <interface.h>
#include <conveyor.h>
#include "messages.h"
#include <can.h>
#include <lcd.h>
#include <interface.h>
#include <timers.h>

/*************************************************************************
*                  PRIORITIES
*************************************************************************/

enum {
    APP_TASK_SENSOR_TWO_PRIO= 4,
    APP_TASK_SENSOR_ONE_PRIO,
    APP_TASK_BLOCK_PLACED_PRI0 ,
    APP_TASK_BLOCK_REMOVED_PRI0,
};

/*************************************************************************
*                  APPLICATION TASK STACKS
*************************************************************************/

enum {
  APP_TASK_SENSOR_TWO_SIZE = 256,
  APP_TASK_BLOCK_PLACED_SIZE = 256,
  APP_TASK_BLOCK_REMOVED_SIZE = 256,
  APP_TASK_SENSOR_ONE_SIZE = 256
};

static OS_STK appTaskSensorTwoStk[APP_TASK_SENSOR_TWO_SIZE];
static OS_STK appTaskBlockPlacedStk[APP_TASK_BLOCK_PLACED_SIZE];
static OS_STK appTaskBlockRemovedStk[APP_TASK_BLOCK_REMOVED_SIZE];
static OS_STK appTaskSensorOneStk[APP_TASK_SENSOR_ONE_SIZE];


/*************************************************************************
*                  APPLICATION FUNCTION PROTOTYPES
*************************************************************************/

static void appTaskSensorTwo(void *pdata);
static void appTaskBlockPlaced(void *pdata);
static void appTaskBlockRemoved(void *pdata);
static void appTaskSensorOne(void *pdata);


/*************************************************************************
*                  GLOBAL VARIABLES
*************************************************************************/
int count = 0;
bool arr;
uint32_t timeElapsed = 0;
/*************************************************************************
*                    GLOBAL FUNCTION DEFINITIONS
*************************************************************************/
static bool startupChecks(void);

int main() {
  /* Initialise the hardware */
  bspInit();
  conveyorInit(); 
  OSInit();               
  initWatch() ;

  /* Create Tasks */
   OSTaskCreate(appTaskBlockPlaced,                               
               (void *)0,
               (OS_STK *)&appTaskBlockPlacedStk[APP_TASK_BLOCK_PLACED_SIZE - 1],
               APP_TASK_BLOCK_PLACED_PRI0);
     
//   OSTaskCreate(appTaskBlockRemoved,                               
//               (void *)0,
//               (OS_STK *)&appTaskBlockRemovedStk[APP_TASK_BLOCK_REMOVED_SIZE - 1],
//                APP_TASK_BLOCK_REMOVED_PRI0);
   
    OSTaskCreate(appTaskSensorTwo,                               
               (void *)0,
               (OS_STK *)&appTaskSensorTwoStk[APP_TASK_SENSOR_TWO_SIZE - 1],
               APP_TASK_SENSOR_TWO_PRIO);
  
    OSTaskCreate(appTaskSensorOne,                               
               (void *)0,
               (OS_STK *)&appTaskSensorOneStk[APP_TASK_SENSOR_ONE_SIZE - 1],
               APP_TASK_SENSOR_ONE_PRIO);
   
     
    if(startupChecks) {    
      OSStart();             
    }
  /* Start the OS */                                              
  /* Should never arrive here */ 
  return 0;      
}

static bool startupChecks(void)
{
//  bool ready;
//  canMessage_t txMsg;
//  canMessage_t rxMsg;
//  while(!ready){
//    if (canReady(CAN_PORT_1)){
//      canRead(CAN_PORT_1, &rxMsg);
//      if (rxMsg.dataA==STARTUP){
//        txMsg.dataA = CONVEYOR_READY;
//        canWrite(CAN_PORT_1, &txMsg);
//      }
//    }
//  }
  return true;
}
/*************************************************************************
*                   APPLICATION TASK DEFINITIONS
*************************************************************************/

static void appTaskSensorTwo(void *pdata) {
  
bool startCounter ;

  osStartTick();
  while (true) {
    if (conveyorItemPresent(CONVEYOR_SENSOR_2) &&(conveyorItemPresent(CONVEYOR_SENSOR_1) ))
    {
      
           conveyorSetState(CONVEYOR_OFF);
    }
    else
    {      
       if (conveyorItemPresent(CONVEYOR_SENSOR_2) )
    {        
            
       if (!startCounter)
    {
       count++;
       startCounter = true;
    }
      
      OSTimeDly(2000);
      conveyorSetState(CONVEYOR_REVERSE);   
    } 
    else
    {
      startCounter = false;
    }     

    if (count > 0 && !conveyorItemPresent(CONVEYOR_SENSOR_1))
    {
        conveyorSetState(CONVEYOR_REVERSE); 
     
    }    
    }
      OSTimeDly(20);

  } 
}

static void appTaskSensorOne(void *pdata) {
  bool startCounter;  
  canMessage_t txMsg;

    
    while(true){  
    if (conveyorItemPresent(CONVEYOR_SENSOR_2) &&(conveyorItemPresent(CONVEYOR_SENSOR_1) ))
    {
           conveyorSetState(CONVEYOR_OFF);
    }
    
    else
    {
       if (conveyorItemPresent(CONVEYOR_SENSOR_1) ) 
    { 
      
      if (!startCounter)
      {
            count --;      
            startCounter = true;
            if(count < 0)
              count = 0;      
       
                 txMsg.id = INPUT_ROBOT_ID;
                 txMsg.dataA = ACK_PLACEBLOCK;
                 txMsg.len = 4;
                 canWrite(CAN_PORT_1, &txMsg);         
      }

      OSTimeDly(200);
      conveyorSetState(CONVEYOR_OFF); 
      
//    timeElapsed = stopWatch () ;//*********************
//    lcdSetTextPos(2,1) ;
//    lcdWrite("%010d " , timeElapsed ) ;
    } 
    
    else{
       startCounter = false;
    }
   
    if(count <= 0)
    {
      conveyorSetState(CONVEYOR_OFF); 
    }
   }     
    OSTimeDly(20);
  }
}

static void appTaskBlockRemoved(void *pdata) {    
    canMessage_t txMsg;
    canMessage_t rxMsg;
    while(true)
    {        
            if (conveyorItemPresent(CONVEYOR_SENSOR_1))
              {
                txMsg.id = OUTPUT_ROBOT_ID;
                txMsg.dataA = REMOVE_BLOCK;
                txMsg.len = 4;
                canWrite(CAN_PORT_1, &txMsg);
              
                if (canReady(CAN_PORT_1)){
                    canRead(CAN_PORT_1, &rxMsg);
                    
                    if(rxMsg.dataA==REMOVE_ATTEMPTED){
                      if(!conveyorItemPresent(CONVEYOR_SENSOR_1)){                      
                       txMsg.id = OUTPUT_ROBOT_ID;
                       txMsg.dataA = REMOVE_OK;
                       txMsg.len = 4;
                       canWrite(CAN_PORT_1, &txMsg);
                       }
                      
                      else{  
                       txMsg.id = OUTPUT_ROBOT_ID;
                       txMsg.dataA = REMOVE_FAILED;
                       txMsg.len = 4;
                       canWrite(CAN_PORT_1, &txMsg);
                       }
                    }                
                }
              }
    OSTimeDly(20);    
    }
}

static void appTaskBlockPlaced(void *pdata) {
    canMessage_t rxMsg;
    canMessage_t txMsg;
    while(true){
      if (canReady(CAN_PORT_1)) {    

        canRead(CAN_PORT_1, &rxMsg);       
         arr = true;

           if(rxMsg.dataA==REQ_PLACEBLOCK){
//             startWatch();//***********************
             if (!conveyorItemPresent(CONVEYOR_SENSOR_2)&& arr == true )
              {
                txMsg.id = INPUT_ROBOT_ID;
                txMsg.dataA = ACK_PLACEBLOCK;
                txMsg.len = 4;
                canWrite(CAN_PORT_1, &txMsg);
                arr = false;            
              } 
         } 
     }    
  }
  OSTimeDly(20);
 }


