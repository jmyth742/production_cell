/* Test Robot
 * Joystick UP    -> inc current joint coordinate
 * Joystick DOWN  -> dec current joint coordinate
 * Joystick RIGHT -> cycle joint selection HAND -> WRIST -> ELBOW -> WAIST
 * Joystick left  -> cycle joint selection HAND <- WRIST <- ELBOW <- WAIST
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

#define BUTTONS_TASK_ID 0
#define N_JOINTS 4

/***************************************************************************
*                       PRIORITIES
***************************************************************************/

enum {APP_TASK_BUTTONS_PRIO = 5};

/****************************************************************************
*                  APPLICATION TASK STACKS
****************************************************************************/

enum {APP_TASK_BUTTONS_STK_SIZE = 256};

static OS_STK appTaskButtonsStk[APP_TASK_BUTTONS_STK_SIZE];

/*****************************************************************************
*                APPLICATION FUNCTION PROTOTYPES
*****************************************************************************/

static void appTaskButtons(void *pdata);

/*****************************************************************************
*                        GLOBAL FUNCTION DEFINITIONS
*****************************************************************************/

int main() {
  /* Initialise the hardware */
  bspInit();
  robotInit();

  /* Initialise the OS */
  OSInit();                                                   

  /* Create Tasks */
  OSTaskCreate(appTaskButtons,                               
               (void *)0,
               (OS_STK *)&appTaskButtonsStk[APP_TASK_BUTTONS_STK_SIZE - 1],
               APP_TASK_BUTTONS_PRIO);

  /* Start the OS */
  OSStart();                                                  
  
  /* Should never arrive here */ 
  return 0;      
}

/**************************************************************************
*                             APPLICATION TASK DEFINITIONS
****************************************************************************/


static void appTaskButtons(void *pdata) {
  
  uint32_t btnState;
  static uint8_t joint = ROBOT_HAND;
  static uint32_t leds[5] = {D1_LED, D1_LED, D2_LED, D3_LED, D4_LED};
  static bool jsRightPressed = false;
  static bool jsLeftPressed = false;
  
  /* Start the OS ticker (highest priority task) */
  osStartTick();

  /* Initialise the display */
  lcdSetTextPos(2, 7);
  lcdWrite("HAND :%08u", robotJointGetState(ROBOT_HAND));
  lcdSetTextPos(2, 8);
  lcdWrite("WRIST:%08u", robotJointGetState(ROBOT_WRIST));
  lcdSetTextPos(2, 9);
  lcdWrite("ELBOW:%08u", robotJointGetState(ROBOT_ELBOW));
  lcdSetTextPos(2, 10);
  lcdWrite("WAIST:%08u", robotJointGetState(ROBOT_WAIST));
  
  /* the main task loop for this task  */
  int led = 1;
  while (true) {

    
    if () {//robotJointGetState(ROBOT_ELBOW) > ELBOW_MAX
      interfaceLedSetState(leds[joint], ~led);
      robotJointSetState(ROBOT_ELBOW, ROBOT_JOINT_POS_INC);
    }
    else if ()) {//isButtonPressedInState(btnState, JS_DOWN
      robotJointSetState(ROBOT_ELBOW, ROBOT_JOINT_POS_DEC);
    }
    OSTimeDly(20);
  }                       
}
