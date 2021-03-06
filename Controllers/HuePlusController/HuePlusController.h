/*---------------------------------------------------------*\
|  Definitions for NZXT Hue+                                |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef LED_STRIP_H
#define LED_STRIP_H

#include "RGBController.h"
#include "serial_port.h"
#include <vector>


#ifndef TRUE
#define TRUE true
#define FALSE false
#endif

#ifndef WIN32
#define LPSTR           char *
#define strtok_s        strtok_r
#endif

#define HUE_PLUS_BAUD           256000
#define HUE_PLUS_PACKET_SIZE    125

enum
{
    HUE_PLUS_CHANNEL_BOTH   = 0x00,     /* Both channels                */
    HUE_PLUS_CHANNEL_1      = 0x01,     /* Channel 1                    */
    HUE_PLUS_CHANNEL_2      = 0x02,     /* Channel 2                    */
    HUE_PLUS_NUM_CHANNELS   = 0x02      /* Number of channels           */
};

enum
{
    HUE_PLUS_CHANNEL_1_IDX  = 0x00,     /* Channel 1 array index        */
    HUE_PLUS_CHANNEL_2_IDX  = 0x01,     /* Channel 2 array index        */
};

enum
{
    HUE_PLUS_SPEED_SLOWEST      = 0x00, /* Slowest speed                */
    HUE_PLUS_SPEED_SLOW         = 0x01, /* Slow speed                   */
    HUE_PLUS_SPEED_NORMAL       = 0x02, /* Normal speed                 */
    HUE_PLUS_SPEED_FAST         = 0x03, /* Fast speed                   */
    HUE_PLUS_SPEED_FASTEST      = 0x04, /* Fastest speed                */
};

enum
{
    HUE_PLUS_MODE_FIXED         = 0x00, /* Fixed colors mode            */
    HUE_PLUS_MODE_FADING        = 0x01, /* Fading mode                  */
    HUE_PLUS_MODE_SPECTRUM      = 0x02, /* Spectrum cycle mode          */
    HUE_PLUS_MODE_MARQUEE       = 0x03, /* Marquee mode                 */
    HUE_PLUS_MODE_COVER_MARQUEE = 0x04, /* Cover marquee mode           */
    HUE_PLUS_MODE_ALTERNATING   = 0x05, /* Alternating mode             */
    HUE_PLUS_MODE_PULSING       = 0x06, /* Pulsing mode                 */
    HUE_PLUS_MODE_BREATHING     = 0x07, /* Breathing mode               */
    HUE_PLUS_MODE_ALERT         = 0x08, /* Alert mode                   */
    HUE_PLUS_MODE_CANDLELIGHT   = 0x09, /* Candlelight mode             */
    HUE_PLUS_MODE_WINGS         = 0x0C, /* Wings mode                   */
    HUE_PLUS_MODE_WAVE          = 0x0D, /* Wave mode                    */
};

class HuePlusController
{
public:
    HuePlusController();
    ~HuePlusController();

    void            Initialize(char* port);
    char*           GetLocation();
    unsigned int    GetLEDsOnChannel(unsigned int channel);
    void            SetChannelLEDs(unsigned char channel, std::vector<RGBColor> colors);
    void            SetMode(unsigned char mode, unsigned char speed, bool direction);
    void            SetModeColors(unsigned char channel, std::vector<RGBColor> colors);

    unsigned int    channel_leds[HUE_PLUS_NUM_CHANNELS];

private:
    char            port_name[128];
    serial_port     *serialport;

    unsigned char   current_mode;
    unsigned char   current_speed;
    bool            current_direction;
};

#endif
