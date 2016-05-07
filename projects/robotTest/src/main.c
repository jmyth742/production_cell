/* Input robot !!

 */
#include <stdbool.h>
#include <ucos_ii.h>
#include <bsp.h>
#include <osutils.h>
#include <leds.h>
#include <buttons.h>
#include <lcd.h>
#include <interface.h>
#include <robot.h>
#include <can.h>
#include "messages.h"
#include <timers.h>

#define BUTTONS_TASK_ID 0
#define N_JOINTS 4
   
enum {
  DUMMY_MIN = 45000,
  HAND_MIN = 45000,
  WRIST_MIN = 45000,
  ELBOW_MIN = 45000,
  WAIST_MIN = 45000,
  DUMMY_MAX = 100000,
  HAND_MAX  = 100000,
  WRIST_MAX = 100000,
  ELBOW_MAX = 100000,
  WAIST_MAX = 100000,
  DUMMY_NEUTRAL = 72500,
  HAND_NEUTRAL = 68750,
  WRIST_NEUTRAL = 82250,
  ELBOW_NEUTRAL = 87500,
  ELBOW_POSITION = 92000,
  WAIST_NEUTRAL = 67250,
  WRIST_PICKUP_UP = 53250,
  WRIST_PICKUP_DOWN = 73500,
  WRIST_DEPOSIT = 79250,
  WAIST_LEFT = 90250,
  
};
   

enum state {//init_checks,
            ready,
            attempt_pickup,
            block_lifted,
            deposit_ready,
            deposit_completed,
            stop_state,
            error_state
             };                              





static void open_hand(void){
   while(robotJointGetState(ROBOT_HAND) > HAND_MIN){
      lcdSetTextPos(2, 7);
      lcdWrite("HAND:%08u", robotJointGetState(ROBOT_HAND));
      robotJointSetState(ROBOT_HAND, ROBOT_JOINT_POS_DEC);  
    }
}

static void close_hand(void){
    while(robotJointGetState(ROBOT_HAND) < HAND_MAX){
      lcdSetTextPos(2, 7);
      lcdWrite("HAND:%08u", robotJointGetState(ROBOT_HAND));
      robotJointSetState(ROBOT_HAND, ROBOT_JOINT_POS_INC);  
    }
}

static void wrist_up(void){
     while(robotJointGetState(ROBOT_WRIST) > WRIST_PICKUP_UP){
      lcdSetTextPos(2, 8);
      lcdWrite("WRIST:%08u", robotJointGetState(ROBOT_WRIST));
      robotJointSetState(ROBOT_WRIST, ROBOT_JOINT_POS_DEC);
      }
}

static void wrist_down(void){
    while(robotJointGetState(ROBOT_WRIST) < WRIST_PICKUP_DOWN){
      lcdSetTextPos(2, 8);
      lcdWrite("WRIST:%08u", robotJointGetState(ROBOT_WRIST));
      robotJointSetState(ROBOT_WRIST, ROBOT_JOINT_POS_INC);  
    }
}

static void waist_left(void){ 
     while(robotJointGetState(ROBOT_WAIST) < WAIST_LEFT){
      lcdSetTextPos(2, 10);
      lcdWrite("WAIST:%08u", robotJointGetState(ROBOT_WAIST));
      robotJointSetState(ROBOT_WAIST, ROBOT_JOINT_POS_INC);  
    }
}

static void waist_right(void){
     while(robotJointGetState(ROBOT_WAIST) > WAIST_MIN){
      lcdSetTextPos(2, 10);
      lcdWrite("WAIST:%08u", robotJointGetState(ROBOT_WAIST));
      robotJointSetState(ROBOT_WAIST, ROBOT_JOINT_POS_DEC);
     }
}

static void elbow_position(void){
     while(robotJointGetState(ROBOT_ELBOW) > ELBOW_POSITION){
      lcdSetTextPos(2, 10);
      lcdWrite("WAIST:%08u", robotJointGetState(ROBOT_WAIST));
      robotJointSetState(ROBOT_WAIST, ROBOT_JOINT_POS_INC);
     }
}



static void init_state(void){
open_hand();
wrist_up(); 
elbow_position();
waist_left();
}

static void pick_up_state(void){
wrist_down();      
close_hand();    
wrist_up();
      }

static void re_attempt_pickup_state(void){
  open_hand();
  wrist_down();
  close_hand();
  wrist_up();
      }


static void deposit_state(void){
  
   wrist_down();
   open_hand();
   wrist_up();
}

//input robot
int main() {
  //intereval time
  uint32_t timeElapsed = 0;
  //global variables
  canMessage_t rxMsg;
  canMessage_t txMsg;
  int current_state=0;
  int tries = 0;
  
 
  /* Initialise the hardware */
  bspInit();
  robotInit();
  initWatch();

  init_state();
 //main while loop for messages
  while(1){
    
   //READY STATE 
     while(current_state==ready){
  interfaceLedToggle(D1_LED);

          if (canReady(CAN_PORT_1)) {  
              
              canRead(CAN_PORT_1, &rxMsg);
          }
          if(rxMsg.id==INPUT_ROBOT_ID){
                 switch(rxMsg.dataA) {
                   //ITEM PRESENT
                 case INPUTPAD_LOADED : 
                     lcdSetTextPos(2, 1);
                     lcdWrite("receive:%04x", rxMsg.dataA);
                    
                     pick_up_state();
                
                     txMsg.id = CONTROLLER_ID;
                     txMsg.len = 4;
                     txMsg.dataA = PICKUP_ATTEMPTED;
                     canWrite(CAN_PORT_1, &txMsg);
                      
                     lcdSetTextPos(2, 2);
                     lcdWrite("transmit:%04x", txMsg.dataA);
                     lcdSetTextPos(2, 3);
                     lcdWrite("id :%08x", txMsg.id);
                     current_state = attempt_pickup;
                  break;
                 //ERROR MESSAGE
                case ERROR :
                            current_state=error_state;
                break; /* optional */
                case STOP :
                     current_state = stop_state;
                break; /* optional */
                                        }
            }
     }
    
      //ATTEMPT PICK UP STATE 
     while(current_state==attempt_pickup){
       interfaceLedToggle(D2_LED);
          lcdSetTextPos(2, 5);
          lcdWrite("state:%02u",current_state);
             if (canReady(CAN_PORT_1)) {  
                  //interfaceLedToggle(D1_LED);
                  canRead(CAN_PORT_1, &rxMsg);
                  lcdSetTextPos(2, 4);
                  lcdWrite("recieve:%04x", rxMsg.dataA);
                  if(rxMsg.id==INPUT_ROBOT_ID){       
                      switch (rxMsg.dataA) {
                        case PICKUP_OK :  
                                 lcdSetTextPos(2, 1);
                                 lcdWrite("rec11:%04x", rxMsg.dataA);
                                 interfaceLedToggle(D1_LED);
                                 waist_right();
                                
                                 txMsg.id = CONVEYOR_ID;
                                 txMsg.len = 4;
                                 txMsg.dataA = REQ_PLACEBLOCK;
                                 canWrite(CAN_PORT_1, &txMsg);
                                      
                                 lcdSetTextPos(2, 2);
                                 lcdWrite("transmit:%04x", txMsg.dataA); 
                                 tries=0;
                                 current_state=block_lifted;
                        break;
                        case PICKUP_FAILED :
                                    tries++;
                                    lcdWrite("receive:%04x", rxMsg.dataA);
                                    re_attempt_pickup_state();
                                    
                                    txMsg.id = CONTROLLER_ID;
                                    txMsg.len = 4;
                                    txMsg.dataA = PICKUP_ATTEMPTED;
                                    canWrite(CAN_PORT_1, &txMsg);
                                    if(tries==3){
                                    lcdSetTextPos(2, 6);
                                    lcdWrite("too many tries");
                                    
                                   }
                                      
                                    current_state=attempt_pickup;
                        break;
                      case ERROR :
                                   current_state=error_state;
                      break; /* optional */
                      case STOP :
                     current_state = stop_state;
                      break; /* optional */
                }
                                     }
                                      }   
   }
   
   //BLOCK LIFTED STATE
     while(current_state==block_lifted){
       interfaceLedToggle(D3_LED);
          lcdSetTextPos(2, 5);
          lcdWrite("state:%02u",current_state);
              if (canReady(CAN_PORT_1)) {  
                      canRead(CAN_PORT_1, &rxMsg);
                      lcdSetTextPos(2, 4);
                      lcdWrite("recieve:%04x", rxMsg.dataA);
                       }
              if(rxMsg.id==INPUT_ROBOT_ID){
                switch (rxMsg.dataA) {
                case ACK_PLACEBLOCK :
                      deposit_state();
                      current_state= deposit_completed;
                break;
                case ERROR :
                     current_state=error_state;
                break; /* optional */
                case STOP :
                     current_state = stop_state;
                break; /* optional */
                      }
   }
     }
     
     //DEPOSIT COMPLETE STATE
     while(current_state==deposit_completed){
              interfaceLedToggle(D4_LED);
              init_state();
              current_state=ready;
    }
    
         while(current_state==error_state){
              robotInit();
              if(canReady(CAN_PORT_1)){
                canRead(CAN_PORT_1, &rxMsg);
                  if(rxMsg.dataA==STARTUP){
                  init_state();
                  current_state=ready;
                  }
              }
  }
  
           while(current_state==stop_state){
              if(canReady(CAN_PORT_1)){
                canRead(CAN_PORT_1, &rxMsg);
                  if(rxMsg.dataA==STARTUP){
                  init_state();
                  current_state=ready;
                  }
              }
  }
  }
  
}