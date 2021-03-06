/*-----------------------------------------*\
|  RGBController_RedragonM711.h             |
|                                           |
|  Generic RGB Interface for Redragon M711  |
|  Cobra RGB Mouse                          |
|                                           |
|  Adam Honse (CalcProgrammer1) 3/25/2020   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "RedragonM711Controller.h"

class RGBController_RedragonM711 : public RGBController
{
public:
    RGBController_RedragonM711(RedragonM711Controller* redragon_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);
    
    void        UpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        UpdateMode();

private:
    RedragonM711Controller*   redragon;
};