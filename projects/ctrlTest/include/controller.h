#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <leds.h>
#include <lcd.h>
#include <buttons.h>
#include <interface.h>
#include <control.h>


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