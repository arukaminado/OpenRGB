#include "RGBController.h"
#include "RGBController_OpenRazerWindows.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hid.h>

#ifdef _WIN64
#define CHROMALINUXDLL        "ChromaDLL64.dll"
#elif WIN32
#define CHROMALINUXDLL        "ChromaDLL.dll"
#endif

/******************************************************************************************\
*                                                                                          *
*   DetectOpenRazerControllers                                                             *
*                                                                                          *
*       Detect devices supported by the OpenRazer kernel drivers                           *
*                                                                                          *
\******************************************************************************************/

void DetectOpenRazerControllers(std::vector<RGBController*> &rgb_controllers)
{
    static HMODULE module = LoadLibrary(CHROMALINUXDLL);

	if(module == nullptr)
    {
        return;
    }

    // map DLL calls
	typedef unsigned int(*INITRAZERDRIVER)(struct hid_device** hdev);
	
	INITRAZERDRIVER init_razer_kbd_driver = reinterpret_cast<INITRAZERDRIVER>(GetProcAddress(module, "init_razer_kbd_driver"));
    static struct device_attribute devkbd_attr_device_type         				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_device_type"));
    static struct device_attribute devkbd_attr_device_serial       				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_device_serial"));
    static struct device_attribute devkbd_attr_firmware_version    				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_firmware_version"));
	static struct device_attribute devkbd_attr_matrix_effect_custom				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_effect_custom"));
	static struct device_attribute devkbd_attr_matrix_custom_frame 				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_custom_frame"));
	static struct device_attribute devkbd_attr_matrix_brightness   				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_brightness"));
	static struct device_attribute devkbd_attr_matrix_effect_none  				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_effect_none"));
	static struct device_attribute devkbd_attr_matrix_effect_static				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_effect_static"));
	static struct device_attribute devkbd_attr_matrix_effect_breath			    = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_effect_breath"));
	static struct device_attribute devkbd_attr_matrix_effect_spectrum			= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_effect_spectrum"));
	static struct device_attribute devkbd_attr_matrix_effect_reactive			= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_effect_reactive"));
	static struct device_attribute devkbd_attr_matrix_effect_wave				= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkbd_attr_matrix_effect_wave"));

	INITRAZERDRIVER init_razer_firefly_driver = reinterpret_cast<INITRAZERDRIVER>(GetProcAddress(module, "init_razer_firefly_driver"));
    static struct device_attribute devfirefly_attr_device_type                  = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_device_type"));
    static struct device_attribute devfirefly_attr_device_serial                = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_device_serial"));
    static struct device_attribute devfirefly_attr_firmware_version             = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_firmware_version"));
	static struct device_attribute devfirefly_attr_matrix_effect_custom         = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_effect_custom"));
	static struct device_attribute devfirefly_attr_matrix_custom_frame          = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_custom_frame"));
	static struct device_attribute devfirefly_attr_matrix_brightness            = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_brightness"));
	static struct device_attribute devfirefly_attr_matrix_effect_none           = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_effect_none"));
	static struct device_attribute devfirefly_attr_matrix_effect_static         = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_effect_static"));
	static struct device_attribute devfirefly_attr_matrix_effect_breath	        = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_effect_breath"));
	static struct device_attribute devfirefly_attr_matrix_effect_spectrum       = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_effect_spectrum"));
	static struct device_attribute devfirefly_attr_matrix_effect_reactive       = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_effect_reactive"));
	static struct device_attribute devfirefly_attr_matrix_effect_wave           = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devfirefly_attr_matrix_effect_wave"));
	
	INITRAZERDRIVER init_razer_mouse_driver = reinterpret_cast<INITRAZERDRIVER>(GetProcAddress(module, "init_razer_mouse_driver"));
    static struct device_attribute devmouse_attr_device_type                   	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_device_type"));
    static struct device_attribute devmouse_attr_device_serial                 	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_device_serial"));
    static struct device_attribute devmouse_attr_firmware_version              	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_firmware_version"));
	static struct device_attribute devmouse_attr_matrix_effect_custom          	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_effect_custom"));
	static struct device_attribute devmouse_attr_matrix_custom_frame           	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_custom_frame"));
	static struct device_attribute devmouse_attr_matrix_brightness             	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_brightness"));
	static struct device_attribute devmouse_attr_logo_led_brightness           	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_logo_led_brightness"));
	static struct device_attribute devmouse_attr_scroll_led_brightness         	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_scroll_led_brightness"));
	static struct device_attribute devmouse_attr_matrix_effect_none            	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_effect_none"));
	static struct device_attribute devmouse_attr_logo_matrix_effect_none       	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_logo_matrix_effect_none"));
	static struct device_attribute devmouse_attr_scroll_matrix_effect_none     	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_scroll_matrix_effect_none"));
	static struct device_attribute devmouse_attr_matrix_effect_static          	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_effect_static"));
	static struct device_attribute devmouse_attr_logo_matrix_effect_static     	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_logo_matrix_effect_static"));
	static struct device_attribute devmouse_attr_scroll_matrix_effect_static   	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_scroll_matrix_effect_static"));
	static struct device_attribute devmouse_attr_matrix_effect_breath 	        = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_effect_breath"));
	static struct device_attribute devmouse_attr_matrix_effect_spectrum        	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_effect_spectrum"));
	static struct device_attribute devmouse_attr_logo_matrix_effect_spectrum   	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_logo_matrix_effect_spectrum"));
	static struct device_attribute devmouse_attr_scroll_matrix_effect_spectrum 	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_scroll_matrix_effect_spectrum"));
	static struct device_attribute devmouse_attr_matrix_effect_reactive        	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_effect_reactive"));
	static struct device_attribute devmouse_attr_logo_matrix_effect_reactive   	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_logo_matrix_effect_reactive"));
	static struct device_attribute devmouse_attr_scroll_matrix_effect_reactive 	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_scroll_matrix_effect_reactive"));
	static struct device_attribute devmouse_attr_scroll_led_effect             	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_scroll_led_effect"));
	static struct device_attribute devmouse_attr_scroll_led_rgb                	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_scroll_led_rgb"));
	static struct device_attribute devmouse_attr_scroll_led_state              	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_scroll_led_state"));
	static struct device_attribute devmouse_attr_matrix_effect_wave            	= *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmouse_attr_matrix_effect_wave"));

	INITRAZERDRIVER init_razer_mug_driver = reinterpret_cast<INITRAZERDRIVER>(GetProcAddress(module, "init_razer_mug_driver"));
    static struct device_attribute devmug_attr_device_type                      = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_device_type"));
    static struct device_attribute devmug_attr_device_serial                    = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_device_serial"));
    static struct device_attribute devmug_attr_firmware_version                 = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_firmware_version"));
	static struct device_attribute devmug_attr_matrix_effect_custom             = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_effect_custom"));
	static struct device_attribute devmug_attr_matrix_custom_frame              = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_custom_frame"));
	static struct device_attribute devmug_attr_matrix_brightness                = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_brightness"));
	static struct device_attribute devmug_attr_matrix_effect_none               = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_effect_none"));
	static struct device_attribute devmug_attr_matrix_effect_static             = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_effect_static"));
	static struct device_attribute devmug_attr_matrix_effect_breath	    	    = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_effect_breath"));
	static struct device_attribute devmug_attr_matrix_effect_spectrum           = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_effect_spectrum"));
	//static struct device_attribute devmug_attr_matrix_effect_reactive         = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_effect_reactive"));
	static struct device_attribute devmug_attr_matrix_effect_wave               = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devmug_attr_matrix_effect_wave"));

	INITRAZERDRIVER init_razer_kraken_driver = reinterpret_cast<INITRAZERDRIVER>(GetProcAddress(module, "init_razer_kraken_driver"));
    static struct device_attribute devkraken_attr_device_type                   = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_device_type"));
    static struct device_attribute devkraken_attr_device_serial                 = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_device_serial"));
    static struct device_attribute devkraken_attr_firmware_version              = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_firmware_version"));
	//static struct device_attribute devkraken_attr_matrix_effect_custom        = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_matrix_effect_custom"));
	//static struct device_attribute devkraken_attr_matrix_custom_frame         = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_matrix_custom_frame"));
	//static struct device_attribute devkraken_attr_matrix_brightness           = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_matrix_brightness"));
	static struct device_attribute devkraken_attr_matrix_effect_none            = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_matrix_effect_none"));
	static struct device_attribute devkraken_attr_matrix_effect_static          = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_matrix_effect_static"));
	static struct device_attribute devkraken_attr_matrix_effect_breath	        = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_matrix_effect_breath"));
	static struct device_attribute devkraken_attr_matrix_effect_spectrum        = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devkraken_attr_matrix_effect_spectrum"));

	INITRAZERDRIVER init_razer_core_driver = reinterpret_cast<INITRAZERDRIVER>(GetProcAddress(module, "init_razer_core_driver"));
    static struct device_attribute devcore_attr_device_type                     = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_device_type"));
    static struct device_attribute devcore_attr_device_serial                   = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_device_serial"));
    static struct device_attribute devcore_attr_firmware_version                = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_firmware_version"));
	static struct device_attribute devcore_attr_matrix_effect_custom            = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_effect_custom"));
	static struct device_attribute devcore_attr_matrix_custom_frame             = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_custom_frame"));
	static struct device_attribute devcore_attr_matrix_brightness               = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_brightness"));
	static struct device_attribute devcore_attr_matrix_effect_none              = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_effect_none"));
	static struct device_attribute devcore_attr_matrix_effect_static            = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_effect_static"));
	static struct device_attribute devcore_attr_matrix_effect_breath	        = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_effect_breath"));
	static struct device_attribute devcore_attr_matrix_effect_spectrum          = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_effect_spectrum"));
	static struct device_attribute devcore_attr_matrix_effect_reactive          = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_effect_reactive"));
	static struct device_attribute devcore_attr_matrix_effect_wave              = *(struct device_attribute*)reinterpret_cast<void*>(GetProcAddress(module, "devcore_attr_matrix_effect_wave"));

	struct hid_device* hdev;
	unsigned int num;

    hdev = NULL;
	num = init_razer_kbd_driver(&hdev);
	for (unsigned int i = 0; i < num; i++)
    {
        device_fn_type* device_fn = new device_fn_type;
        device_fn->device_type             	= &devkbd_attr_device_type;
        device_fn->device_serial           	= &devkbd_attr_device_serial;
		device_fn->firmware_version			= &devkbd_attr_firmware_version;
        device_fn->matrix_effect_custom    	= &devkbd_attr_matrix_effect_custom;
        device_fn->matrix_custom_frame     	= &devkbd_attr_matrix_custom_frame;
        device_fn->matrix_brightness       	= &devkbd_attr_matrix_brightness;
        device_fn->matrix_effect_none      	= &devkbd_attr_matrix_effect_none;
        device_fn->matrix_effect_static    	= &devkbd_attr_matrix_effect_static;
		device_fn->matrix_effect_breath	    = &devkbd_attr_matrix_effect_breath;
        device_fn->matrix_effect_spectrum  	= &devkbd_attr_matrix_effect_spectrum;
        device_fn->matrix_effect_reactive  	= &devkbd_attr_matrix_effect_reactive;
		device_fn->matrix_effect_wave		= &devkbd_attr_matrix_effect_wave;

        RGBController_OpenRazer * razer_rgb = new RGBController_OpenRazer(&hdev[i].dev, device_fn);

        if(razer_rgb->device_index != -1)
        {
            rgb_controllers.push_back(razer_rgb);
        }
        else
        {
            delete razer_rgb;
        }
    }

    hdev = NULL;
	num = init_razer_mouse_driver(&hdev);
	for (unsigned int i = 0; i < num; i++)
    {
        device_fn_type* device_fn = new device_fn_type;
        device_fn->device_type             	= &devmouse_attr_device_type;
        device_fn->device_serial           	= &devmouse_attr_device_serial;
		device_fn->firmware_version			= &devmouse_attr_firmware_version;
        device_fn->matrix_effect_custom    	= &devmouse_attr_matrix_effect_custom;
        device_fn->matrix_custom_frame     	= &devmouse_attr_matrix_custom_frame;
        device_fn->matrix_brightness       	= &devmouse_attr_matrix_brightness;
        device_fn->matrix_effect_none      	= &devmouse_attr_matrix_effect_none;
        device_fn->matrix_effect_static    	= &devmouse_attr_matrix_effect_static;
		device_fn->matrix_effect_breath	    = &devmouse_attr_matrix_effect_breath;
        device_fn->matrix_effect_spectrum  	= &devmouse_attr_matrix_effect_spectrum;
        device_fn->matrix_effect_reactive  	= &devmouse_attr_matrix_effect_reactive;
		device_fn->matrix_effect_wave		= &devmouse_attr_matrix_effect_wave;

        RGBController_OpenRazer * razer_rgb = new RGBController_OpenRazer(&hdev[i].dev, device_fn);

        if(razer_rgb->device_index != -1)
        {
            rgb_controllers.push_back(razer_rgb);
        }
        else
        {
            delete razer_rgb;
        }
    }

    hdev = NULL;
	num = init_razer_firefly_driver(&hdev);
	for (unsigned int i = 0; i < num; i++)
    {
        device_fn_type* device_fn = new device_fn_type;
        device_fn->device_type             	= &devfirefly_attr_device_type;
        device_fn->device_serial           	= &devfirefly_attr_device_serial;
		device_fn->firmware_version			= &devfirefly_attr_firmware_version;
        device_fn->matrix_effect_custom    	= &devfirefly_attr_matrix_effect_custom;
        device_fn->matrix_custom_frame     	= &devfirefly_attr_matrix_custom_frame;
        device_fn->matrix_brightness       	= &devfirefly_attr_matrix_brightness;
        device_fn->matrix_effect_none      	= &devfirefly_attr_matrix_effect_none;
        device_fn->matrix_effect_static    	= &devfirefly_attr_matrix_effect_static;
		device_fn->matrix_effect_breath	    = &devfirefly_attr_matrix_effect_breath;
        device_fn->matrix_effect_spectrum  	= &devfirefly_attr_matrix_effect_spectrum;
        device_fn->matrix_effect_reactive  	= &devfirefly_attr_matrix_effect_reactive;
		device_fn->matrix_effect_wave		= &devfirefly_attr_matrix_effect_wave;

        RGBController_OpenRazer * razer_rgb = new RGBController_OpenRazer(&hdev[i].dev, device_fn);

        if(razer_rgb->device_index != -1)
        {
            rgb_controllers.push_back(razer_rgb);
        }
        else
        {
            delete razer_rgb;
        }
    }

    hdev = NULL;
	num = init_razer_mug_driver(&hdev);
	for (unsigned int i = 0; i < num; i++)
    {
        device_fn_type* device_fn = new device_fn_type;
        device_fn->device_type               = &devmug_attr_device_type;
        device_fn->device_serial             = &devmug_attr_device_serial;
		device_fn->firmware_version			 = &devmug_attr_firmware_version;
        device_fn->matrix_effect_custom      = &devmug_attr_matrix_effect_custom;
        device_fn->matrix_custom_frame       = &devmug_attr_matrix_custom_frame;
        device_fn->matrix_brightness         = &devmug_attr_matrix_brightness;
        device_fn->matrix_effect_none        = &devmug_attr_matrix_effect_none;
        device_fn->matrix_effect_static      = &devmug_attr_matrix_effect_static;
		device_fn->matrix_effect_breath	     = &devmug_attr_matrix_effect_breath;
        device_fn->matrix_effect_spectrum    = &devmug_attr_matrix_effect_spectrum;
        device_fn->matrix_effect_reactive    = NULL;//&devmug_attr_matrix_effect_reactive;
		device_fn->matrix_effect_wave        = &devmug_attr_matrix_effect_wave;

        RGBController_OpenRazer * razer_rgb = new RGBController_OpenRazer(&hdev[i].dev, device_fn);

        if(razer_rgb->device_index != -1)
        {
            rgb_controllers.push_back(razer_rgb);
        }
        else
        {
            delete razer_rgb;
        }
    }



}   /* DetectOpenRazerControllers() */
