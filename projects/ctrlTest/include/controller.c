#include "controller.h"

void readyLightToggle()
{
  interfaceLedToggle(D1_LED);
}

void runningLightToggle()
{
  interfaceLedToggle(D2_LED);
}

void pausedLightToggle()
{
  interfaceLedToggle(D3_LED);
}

void emergencyLightToggle()
{
  controlAlarmToggleState();
}

bool startBtn()
{
  return isButtonPressed(BUT_1);
}

bool pauseBtn()
{
  return isButtonPressed(BUT_2);
}

bool errorBtn()
{
  return isButtonPressed(JS_CENTRE);
}

bool emergencyBtn()
{
  return controlEmergencyStopButtonPressed();
}




