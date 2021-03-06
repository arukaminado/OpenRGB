/*-----------------------------------------*\
|  RGBController_MSIRGB.h                   |
|                                           |
|  Generic RGB Interface for MSI-RGB        |
|                                           |
|  Adam Honse (CalcProgrammer1) 2/14/2020   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "MSIRGBController.h"

class RGBController_MSIRGB : public RGBController
{
public:
    RGBController_MSIRGB(MSIRGBController* msi_ptr);
    ~RGBController_MSIRGB();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        UpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        UpdateMode();

private:
    MSIRGBController*       msi;
};
