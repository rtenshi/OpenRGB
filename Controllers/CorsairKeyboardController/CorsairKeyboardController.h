/*-----------------------------------------*\
|  CorsairKeyboardController.h              |
|                                           |
|  Definitions and types for Corsair RGB    |
|  keyboard lighting controller             |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/9/2020    |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

enum
{
    CORSAIR_COMMAND_WRITE       = 0x07,
    CORSAIR_COMMAND_READ        = 0x0E,
    CORSAIR_COMMAND_STREAM      = 0x7F
};

enum
{
    CORSAIR_PROPERTY_FIRMWARE_INFO              = 0x01,
    CORSAIR_PROPERTY_RESET                      = 0x02,
    CORSAIR_PROPERTY_SPECIAL_FUNCTION           = 0x04,
    CORSAIR_PROPERTY_LIGHTING_CONTROL           = 0x05,
    CORSAIR_PROPERTY_HARDWARE_PROFILE           = 0x13,
    CORSAIR_PROPERTY_SUBMIT_MOUSE_COLOR         = 0x22,
    CORSAIR_PROPERTY_SUBMIT_KEYBOARD_COLOR_9    = 0x27,
    CORSAIR_PROPERTY_SUBMIT_KEYBOARD_COLOR_24   = 0x28,
};

enum
{
    CORSAIR_LIGHTING_CONTROL_HARDWARE           = 0x01,
    CORSAIR_LIGHTING_CONTROL_SOFTWARE           = 0x02
};

enum
{
    CORSAIR_COLOR_CHANNEL_RED       = 0x01,
    CORSAIR_COLOR_CHANNEL_GREEN     = 0x02,
    CORSAIR_COLOR_CHANNEL_BLUE      = 0x03
};

class CorsairKeyboardController
{
public:
    CorsairKeyboardController(hid_device* dev_handle);
    ~CorsairKeyboardController();

    void SetLEDsDirect(std::vector<RGBColor> colors);
    void SetLEDs(std::vector<RGBColor> colors);

private:
    hid_device*             dev;

    void    LightingControl();

    void    ReadFirmwareInfo();
    
    void    StreamPacket
                (
                unsigned char   packet_id,
                unsigned char   data_sz,
                unsigned char*  data_ptr
                );

    void    SubmitColors();

    void    SubmitLimitedColors
                (
                unsigned char   byte_count
                );
};
