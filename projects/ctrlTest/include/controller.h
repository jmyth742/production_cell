#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <leds.h>
#include <lcd.h>
#include <buttons.h>
#include <interface.h>
#include <control.h>

typedef enum ctrlButtons{
  TogStart,
  TogPause,
  Error,
  Reset
} Button;

typedef enum lights{
  Ready,
  Running,
  Paused,
  Emergency
} Lamp;

bool toggleStart(void);

void readyLightToggle(void);
void runningLightToggle(void);
void pausedLightToggle(void);
void emergencyLightToggle(void);

bool startBtn(void);
bool pauseBtn(void);
bool errorBtn(void);
bool emergencyBtn(void);

#endif