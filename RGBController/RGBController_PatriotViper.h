/*-----------------------------------------*\
|  RGBController_PatriotViper.h             |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  Patriot Viper RGB interface              |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/1/2020    |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "PatriotViperController.h"

class RGBController_PatriotViper : public RGBController
{
public:
    RGBController_PatriotViper(PatriotViperController* viper_ptr);
    void        UpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        UpdateMode();

private:
    PatriotViperController* viper;
};
