/*-----------------------------------------*\
|  CorsairKeyboardController.cpp            |
|                                           |
|  Driver for Corsair RGB keyboard lighting |
|  controller                               |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/9/2020    |
\*-----------------------------------------*/

#include "CorsairKeyboardController.h"

#include <cstring>

static unsigned int keys[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0C, 0x0D, 0x0E, 0x0F, 0x11, 0x12,
                              0x14, 0x15, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x24, 0x25, 0x26,
                              0x27, 0x28, 0x2A, 0x2B, 0x2C, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
                              0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x42, 0x43, 0x44, 0x45, 0x48, 73,   74,   75,   76,   78,
                              79,   80,   81,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   96,   97,
                              98,   99,   100,  101,  102,  103,  104,  105,  108,  109,  110,  111,  112,  113,  115,
                              116,  117,  120,  121,  122,  123,  124,  126,  127,  128,  129,  132,  133,  134,  135,
                              136,  137,  139,  140,  141};

static void send_usb_msg(hid_device* dev, char * data_pkt)
{
    char usb_pkt[65];
    usb_pkt[0] = 0x00;
    for(int i = 1; i < 65; i++)
    {
        usb_pkt[i] = data_pkt[i-1];
    }
    int bytes = hid_send_feature_report(dev, (unsigned char *)usb_pkt, 65);
    bytes++;
}

static void get_usb_msg(hid_device* dev, char* data_pkt)
{
    char usb_pkt[65];
    usb_pkt[0] = 0x00;
    for(int i = 1; i < 65; i++)
    {
        usb_pkt[i] = data_pkt[i-1];
    }
    int bytes = hid_get_feature_report(dev, (unsigned char*)data_pkt, 64);
    bytes++;
}

CorsairKeyboardController::CorsairKeyboardController(hid_device* dev_handle)
{
    dev = dev_handle;

    char data_pkt[64] = { 0 };

    LightingControl();
}

CorsairKeyboardController::~CorsairKeyboardController()
{

}

void CorsairKeyboardController::SetLEDs(std::vector<RGBColor> colors)
{
    unsigned char data_pkt[216];
    unsigned char red_val[144];
    unsigned char grn_val[144];
    unsigned char blu_val[144];

    /*-----------------------------------------------------*\
    | Zero out buffers                                      |
    \*-----------------------------------------------------*/
    memset(data_pkt, 0x00, sizeof( data_pkt ));
    memset(red_val, 0x00, sizeof( red_val ));
    memset(grn_val, 0x00, sizeof( grn_val ));
    memset(blu_val, 0x00, sizeof( blu_val ));

    /*-----------------------------------------------------*\
    | Scale color values to 9-bit                           |
    \*-----------------------------------------------------*/
    for(int color_idx = 0; color_idx < colors.size(); color_idx++)
    {
        RGBColor      color = colors[color_idx];
        unsigned char red   = RGBGetRValue(color);
        unsigned char grn   = RGBGetGValue(color);
        unsigned char blu   = RGBGetBValue(color);

        if( red > 7 ) red = 7;
        if( grn > 7 ) grn = 7;
        if( blu > 7 ) blu = 7;

        red = 7 - red;
        grn = 7 - grn;
        blu = 7 - blu;

        red_val[keys[color_idx]] = red;
        grn_val[keys[color_idx]] = grn;
        blu_val[keys[color_idx]] = blu;
    }

    /*-----------------------------------------------------*\
    | Pack the color values, 2 values per byte              |
    \*-----------------------------------------------------*/
    for(int red_idx = 0; red_idx < 72; red_idx++)
    {
        data_pkt[red_idx] = red_val[(red_idx * 2) + 1] << 4 | red_val[red_idx * 2];
    }

    for(int grn_idx = 0; grn_idx < 72; grn_idx++)
    {
        data_pkt[grn_idx + 72] = grn_val[(grn_idx * 2) + 1] << 4 | grn_val[grn_idx * 2];
    }

    for(int blu_idx = 0; blu_idx < 72; blu_idx++)
    {
        data_pkt[blu_idx + 144] = blu_val[(blu_idx * 2) + 1] << 4 | blu_val[blu_idx * 2];
    }

    /*-----------------------------------------------------*\
    | Send the packets                                      |
    \*-----------------------------------------------------*/
    StreamPacket(1, 60, &data_pkt[0]);
    StreamPacket(2, 60, &data_pkt[60]);
    StreamPacket(3, 60, &data_pkt[120]);
    StreamPacket(4, 36, &data_pkt[180]);
    
    SubmitLimitedColors(216);
}

/*-------------------------------------------------------------------------------------------------*\
| Private packet sending functions.                                                                 |
\*-------------------------------------------------------------------------------------------------*/

void CorsairKeyboardController::LightingControl()
{
    char usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Lighting Control packet                        |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_COMMAND_WRITE;
    usb_buf[0x01]   = CORSAIR_PROPERTY_LIGHTING_CONTROL;
    usb_buf[0x02]   = CORSAIR_LIGHTING_CONTROL_SOFTWARE;
    usb_buf[0x04]   = 0x03;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    send_usb_msg(dev, usb_buf);
}

void CorsairKeyboardController::ReadFirmwareInfo()
{
    char usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Read Firmware Info packet                      |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_COMMAND_READ;
    usb_buf[0x01]   = CORSAIR_PROPERTY_FIRMWARE_INFO;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    send_usb_msg(dev, usb_buf);
    get_usb_msg(dev, usb_buf);
}

void CorsairKeyboardController::StreamPacket
    (
    unsigned char   packet_id,
    unsigned char   data_sz,
    unsigned char*  data_ptr
    )
{
    char usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Stream packet                                  |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_COMMAND_STREAM;
    usb_buf[0x01]   = packet_id;
    usb_buf[0x02]   = data_sz;

    /*-----------------------------------------------------*\
    | Copy in data bytes                                    |
    \*-----------------------------------------------------*/
    memcpy(&usb_buf[0x04], data_ptr, data_sz);

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    send_usb_msg(dev, usb_buf);
}

void CorsairKeyboardController::SubmitColors()
{
    char usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Submit Keyboard 24-Bit Colors packet           |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_COMMAND_WRITE;
    usb_buf[0x01]   = CORSAIR_PROPERTY_SUBMIT_KEYBOARD_COLOR_24;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    send_usb_msg(dev, usb_buf);
}

void CorsairKeyboardController::SubmitLimitedColors
    (
    unsigned char   byte_count
    )
{
    char usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Submit Keyboard 9-Bit Colors packet            |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = CORSAIR_COMMAND_WRITE;
    usb_buf[0x01]   = CORSAIR_PROPERTY_SUBMIT_KEYBOARD_COLOR_9;
    usb_buf[0x04]   = byte_count;

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    send_usb_msg(dev, usb_buf);
}