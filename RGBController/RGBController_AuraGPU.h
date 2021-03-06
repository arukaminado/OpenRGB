/*-----------------------------------------*\
|  RGBController_AuraGPU.h                  |
|                                           |
|  Generic RGB Interface for Asus Aura GPU  |
|                                           |
|  Jan Rettig (Klapstuhl) 14.02.2020        |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "AuraGPUController.h"

class RGBController_AuraGPU : public RGBController
{
public:
    RGBController_AuraGPU(AuraGPUController* aura_gpu_ptr);

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        UpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        UpdateMode();

private:
    AuraGPUController* aura_gpu;

    int        GetDeviceMode();
};
