#include "controller.h"
static bool isReady, isRunning;  
bool toggleStart()
{
    if(isReady)
    {
      if(!isRunning)
      {
        isRunning = true;
        return true;
      }
      else
      {
        return false;
      }
    }
    return false;
}


void readyLightToggle()
{
  ledToggle(USB_LINK_LED);
}

void runningLightToggle()
{
  ledToggle(USB_CONNECT_LED);
}

void pausedLightToggle()
{
  interfaceLedToggle(D1_LED);
}

void emergencyLightToggle()
{
  controlAlarmToggleState();
}

bool startBtn()
{
  isButtonPressed(BUT_1);
}

bool pauseBtn()
{
  isButtonPressed(BUT_2);
}

bool errorBtn()
{
  isButtonPressed(JS_CENTRE);
}

bool emergencyBtn()
{
  controlEmergencyStopButtonPressed();
}




