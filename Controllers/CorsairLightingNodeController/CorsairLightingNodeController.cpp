/*---------------------------------------------------------*\
|  Processing Code for Corsair Lighting Node Pro            |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 1/12/2020        |
\*---------------------------------------------------------*/

#include "CorsairLightingNodeController.h"

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

//Include thread libraries for Windows or Linux
#ifdef WIN32
#include <process.h>
#else
#include "pthread.h"
#include "unistd.h"
#endif

//Thread functions have different types in Windows and Linux
#ifdef WIN32
#define THREAD static void
#define THREADRETURN
#else
#define THREAD static void*
#define THREADRETURN return(NULL);
#endif

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>

static void Sleep(unsigned int milliseconds)
{
    usleep(1000 * milliseconds);
}
#endif

THREAD keepalive_thread(void *param)
{
    CorsairLightingNodeController* corsair = static_cast<CorsairLightingNodeController*>(param);
    corsair->KeepaliveThread();
    THREADRETURN
}

CorsairLightingNodeController::CorsairLightingNodeController(libusb_device_handle* dev_handle, unsigned int dev_endpoint)
{
    dev = dev_handle;
    endpoint = dev_endpoint;

    SendFirmwareRequest();

    /*-----------------------------------------------------*\
    | The Corsair Lighting Node Pro requires a packet within|
    | 20 seconds of sending the lighting change in order    |
    | to not revert back into rainbow mode.  Start a thread |
    | to continuously send a keepalive packet every 5s      |
    \*-----------------------------------------------------*/
#ifdef WIN32
    _beginthread(keepalive_thread, 0, this);
#else
    pthread_t thread;
    pthread_create(&thread, NULL, &keepalive_thread, this);
#endif
}

CorsairLightingNodeController::~CorsairLightingNodeController()
{
}

void CorsairLightingNodeController::KeepaliveThread()
{
    while(1)
    {
        SendCommit();
        Sleep(5000);
    }
}

std::string CorsairLightingNodeController::GetFirmwareString()
{
    return(firmware_version);
}

void CorsairLightingNodeController::SetChannelEffect(unsigned char channel,
                                                unsigned char num_leds,
                                                unsigned char mode,
                                                unsigned char speed,
                                                unsigned char direction,
                                                bool          random,
                                                unsigned char red1,
                                                unsigned char grn1,
                                                unsigned char blu1,
                                                unsigned char red2,
                                                unsigned char grn2,
                                                unsigned char blu2,
                                                unsigned char red3,
                                                unsigned char grn3,
                                                unsigned char blu3
                                               )
{
    /*-----------------------------------------------------*\
    | Send Reset packet                                     |
    \*-----------------------------------------------------*/
    SendReset(channel);

    /*-----------------------------------------------------*\
    | Send Begin packet                                     |
    \*-----------------------------------------------------*/
    SendBegin(channel);

    /*-----------------------------------------------------*\
    | Set Port State packet                                 |
    \*-----------------------------------------------------*/
    SendPortState(channel, CORSAIR_LIGHTING_NODE_PORT_STATE_HARDWARE);

    /*-----------------------------------------------------*\
    | Set Effect Configuration packet                       |
    \*-----------------------------------------------------*/
    SendEffectConfig
    (
        channel,
        0,
        num_leds,
        mode,
        speed,
        direction,
        random,
        red1,
        grn1,
        blu1,
        red2,
        grn2,
        blu2,
        red3,
        grn3,
        blu3,
        0,
        0,
        0
    );

    /*-----------------------------------------------------*\
    | Send Commit packet                                    |
    \*-----------------------------------------------------*/
    SendCommit();
}

void CorsairLightingNodeController::SetChannelLEDs(unsigned char channel, RGBColor * colors, unsigned int num_colors)
{
    unsigned char   color_data[50];
    unsigned char   pkt_max;

    /*-----------------------------------------------------*\
    | Send Port State packet                                |
    \*-----------------------------------------------------*/
    SendPortState(channel, CORSAIR_LIGHTING_NODE_PORT_STATE_SOFTWARE);

    /*-----------------------------------------------------*\
    | Send red channel packet 1                             |
    \*-----------------------------------------------------*/
    pkt_max = 50;

    if(pkt_max > num_colors)
    {
        pkt_max = (unsigned char)num_colors;
    }

    for(int idx = 0; idx < pkt_max; idx++)
    {
        color_data[idx] = RGBGetRValue(colors[idx]);
    }

    SendDirect(channel, 0, pkt_max, CORSAIR_LIGHTING_NODE_DIRECT_CHANNEL_RED, color_data);

    /*-----------------------------------------------------*\
    | Send red channel packet 2 if necessary                |
    \*-----------------------------------------------------*/
    pkt_max = 0;

    if (num_colors > 50)
    {
        pkt_max = (unsigned char)(num_colors - 50);
    }

    if(pkt_max > 0)
    {
        for (std::size_t idx = 0; idx < pkt_max; idx++)
        {
            color_data[idx] = RGBGetRValue(colors[idx+50]);
        }

        SendDirect(channel, 50, pkt_max, CORSAIR_LIGHTING_NODE_DIRECT_CHANNEL_RED, color_data);
    }

    /*-----------------------------------------------------*\
    | Send green channel packet 1                           |
    \*-----------------------------------------------------*/
    pkt_max = 50;

    if(pkt_max > num_colors)
    {
        pkt_max = (unsigned char)num_colors;
    }

    for(int idx = 0; idx < pkt_max; idx++)
    {
        color_data[idx] = RGBGetGValue(colors[idx]);
    }

    SendDirect(channel, 0, pkt_max, CORSAIR_LIGHTING_NODE_DIRECT_CHANNEL_GREEN, color_data);

    /*-----------------------------------------------------*\
    | Send green channel packet 2 if necessary              |
    \*-----------------------------------------------------*/
    pkt_max = 0;

    if (num_colors > 50)
    {
        pkt_max = (unsigned char)(num_colors - 50);
    }

    if(pkt_max > 0)
    {
        for (std::size_t idx = 0; idx < pkt_max; idx++)
        {
            color_data[idx] = RGBGetGValue(colors[idx+50]);
        }

        SendDirect(channel, 50, pkt_max, CORSAIR_LIGHTING_NODE_DIRECT_CHANNEL_GREEN, color_data);
    }

    /*-----------------------------------------------------*\
    | Send blue channel packet 1                            |
    \*-----------------------------------------------------*/
    pkt_max = 50;

    if(pkt_max > num_colors)
    {
        pkt_max = (unsigned char)num_colors;
    }

    for(int idx = 0; idx < pkt_max; idx++)
    {
        color_data[idx] = RGBGetBValue(colors[idx]);
    }

    SendDirect(channel, 0, pkt_max, CORSAIR_LIGHTING_NODE_DIRECT_CHANNEL_BLUE, color_data);

    /*-----------------------------------------------------*\
    | Send blue channel packet 2 if necessary               |
    \*-----------------------------------------------------*/
    pkt_max = 0;

    if (num_colors > 50)
    {
        pkt_max = (unsigned char)(num_colors - 50);
    }

    if(pkt_max > 0)
    {
        for (std::size_t idx = 0; idx < pkt_max; idx++)
        {
            color_data[idx] = RGBGetBValue(colors[idx+50]);
        }

        SendDirect(channel, 50, pkt_max, CORSAIR_LIGHTING_NODE_DIRECT_CHANNEL_BLUE, color_data);
    }

    /*-----------------------------------------------------*\
    | Send Commit packet                                    |
    \*-----------------------------------------------------*/
    SendCommit();
}

/*-------------------------------------------------------------------------------------------------*\
| Private packet sending functions.                                                                 |
\*-------------------------------------------------------------------------------------------------*/

void CorsairLightingNodeController::SendFirmwareRequest()
{
    int             actual;
    unsigned char   usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Firmware Version Request packet                |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_LIGHTING_NODE_PACKET_ID_FIRMWARE;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    libusb_interrupt_transfer(dev, endpoint, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x80 + endpoint, usb_buf, 64, &actual, 0);

    if(actual > 0)
    {
        firmware_version = std::to_string(usb_buf[0x01]) + "." + std::to_string(usb_buf[0x02]) + "." + std::to_string(usb_buf[0x03]);
    }
}

void CorsairLightingNodeController::SendDirect
    (
    unsigned char   channel,
    unsigned char   start,
    unsigned char   count,
    unsigned char   color_channel,
    unsigned char*  color_data
    )
{
    int             actual;
    unsigned char   usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Direct packet                                  |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_LIGHTING_NODE_PACKET_ID_DIRECT;
    usb_buf[0x01]   = channel;
    usb_buf[0x02]   = start;
    usb_buf[0x03]   = count;
    usb_buf[0x04]   = color_channel;

    /*-----------------------------------------------------*\
    | Copy in color data bytes                              |
    \*-----------------------------------------------------*/
    memcpy(&usb_buf[0x05], color_data, count);

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    libusb_interrupt_transfer(dev, endpoint, usb_buf, 64, &actual, 0);
}

void CorsairLightingNodeController::SendCommit()
{
    int             actual;
    unsigned char   usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Commit packet                                  |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_LIGHTING_NODE_PACKET_ID_COMMIT;
    usb_buf[0x01]   = 0xFF;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    libusb_interrupt_transfer(dev, endpoint, usb_buf, 64, &actual, 0);
}

void CorsairLightingNodeController::SendBegin
    (
    unsigned char   channel
    )
{
    int             actual;
    unsigned char   usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Begin packet                                   |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_LIGHTING_NODE_PACKET_ID_BEGIN;
    usb_buf[0x01]   = channel;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    libusb_interrupt_transfer(dev, endpoint, usb_buf, 64, &actual, 0);
}

void CorsairLightingNodeController::SendEffectConfig
    (
    unsigned char   channel,
    unsigned char   count,
    unsigned char   led_type,
    unsigned char   mode,
    unsigned char   speed,
    unsigned char   direction,
    unsigned char   change_style,
    unsigned char   color_0_red,
    unsigned char   color_0_green,
    unsigned char   color_0_blue,
    unsigned char   color_1_red,
    unsigned char   color_1_green,
    unsigned char   color_1_blue,
    unsigned char   color_2_red,
    unsigned char   color_2_green,
    unsigned char   color_2_blue,
    unsigned short  temperature_0,
    unsigned short  temperature_1,
    unsigned short  temperature_2
    )
{
    int             actual;
    unsigned char   usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Effect Config packet                           |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_LIGHTING_NODE_PACKET_ID_EFFECT_CONFIG;
    usb_buf[0x01]   = channel;
    usb_buf[0x02]   = count * 10;
    usb_buf[0x03]   = led_type;

    /*-----------------------------------------------------*\
    | Set up mode parameters                                |
    \*-----------------------------------------------------*/
    usb_buf[0x04]   = mode;
    usb_buf[0x05]   = speed;
    usb_buf[0x06]   = direction;
    usb_buf[0x07]   = change_style;
    usb_buf[0x08]   = 0;

    /*-----------------------------------------------------*\
    | Set up mode colors                                    |
    \*-----------------------------------------------------*/
    usb_buf[0x09]   = color_0_red;
    usb_buf[0x10]   = color_0_green;
    usb_buf[0x11]   = color_0_blue;
    usb_buf[0x12]   = color_1_red;
    usb_buf[0x13]   = color_1_green;
    usb_buf[0x14]   = color_1_blue;
    usb_buf[0x15]   = color_2_red;
    usb_buf[0x16]   = color_2_green;
    usb_buf[0x17]   = color_2_blue;

    /*-----------------------------------------------------*\
    | Set up temperatures                                   |
    \*-----------------------------------------------------*/
    usb_buf[0x12]   = (temperature_0 >> 8);
    usb_buf[0x13]   = (temperature_0 & 0xFF);
    usb_buf[0x14]   = (temperature_1 >> 8);
    usb_buf[0x15]   = (temperature_1 & 0xFF);
    usb_buf[0x16]   = (temperature_2 >> 8);
    usb_buf[0x17]   = (temperature_2 & 0xFF);

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    libusb_interrupt_transfer(dev, endpoint, usb_buf, 64, &actual, 0);
}

void CorsairLightingNodeController::SendTemperature()
{

}

void CorsairLightingNodeController::SendReset
    (
    unsigned char   channel
    )
{
    int             actual;
    unsigned char   usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Reset packet                                   |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_LIGHTING_NODE_PACKET_ID_RESET;
    usb_buf[0x01]   = channel;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    libusb_interrupt_transfer(dev, endpoint, usb_buf, 64, &actual, 0);
}

void CorsairLightingNodeController::SendPortState
    (
    unsigned char   channel,
    unsigned char   state
    )
{
    int             actual;
    unsigned char   usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Port State packet                              |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_LIGHTING_NODE_PACKET_ID_PORT_STATE;
    usb_buf[0x01]   = channel;
    usb_buf[0x02]   = state;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    libusb_interrupt_transfer(dev, endpoint, usb_buf, 64, &actual, 0);
}

void CorsairLightingNodeController::SendBrightness()
{

}

void CorsairLightingNodeController::SendLEDCount()
{

}

void CorsairLightingNodeController::SendProtocol()
{

}
