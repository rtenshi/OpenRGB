/*-----------------------------------------*\
|  HyperXController.cpp                     |
|                                           |
|  Definitions and types for HyperX Predator|
|  RGB RAM lighting controller              |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/29/2019   |
\*-----------------------------------------*/

#include "HyperXController.h"
#include <cstring>

HyperXController::HyperXController(i2c_smbus_interface* bus, hyperx_dev_id dev, unsigned char slots)
{
    this->bus   = bus;
    this->dev   = dev;
    slots_valid = slots;

    strcpy(device_name, "HyperX Predator RGB");

    led_count = 0;

    for(int i = 0; i < 8; i++)
    {
        if((slots_valid & (1 << i)) != 0)
        {
            led_count += 5;
        }
    }

    mode = HYPERX_MODE_DIRECT;
}

HyperXController::~HyperXController()
{

}

std::string HyperXController::GetDeviceName()
{
    return(device_name);
}

std::string HyperXController::GetDeviceLocation()
{
    std::string return_string(bus->device_name);
    char addr[5];
    snprintf(addr, 5, "0x%02X", dev);
    return_string.append(", address ");
    return_string.append(addr);
    return(return_string);
}

unsigned int HyperXController::GetLEDCount()
{
    return(led_count);
}

unsigned int HyperXController::GetSlotCount()
{
    unsigned int slot_count = 0;

    for(int slot = 0; slot < 4; slot++)
    {
        if((slots_valid & (1 << slot)) != 0)
        {
            slot_count++;
        }
    }

    return(slot_count);
}

unsigned int HyperXController::GetMode()
{
    return(mode);
}

void HyperXController::SetEffectColor(unsigned char red, unsigned char green, unsigned char blue)
{
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x01);

    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_EFFECT_RED,        red  );
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_EFFECT_GREEN,      green);
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_EFFECT_BLUE,       blue );
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_EFFECT_BRIGHTNESS, 0x64 );

    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x02);
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x03);
}

void HyperXController::SetAllColors(unsigned char red, unsigned char green, unsigned char blue)
{
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x01);

    /*-----------------------------------------------------*\
    | Loop through all slots and only set those which are   |
    | active.                                               |
    \*-----------------------------------------------------*/
    for(int slot = 0; slot < 4; slot++)
    {
        if((slots_valid & (1 << slot)) != 0)
        {
            unsigned char base        = slot_base[slot];
            unsigned char red_base    = base + 0x00;
            unsigned char green_base  = base + 0x01;
            unsigned char blue_base   = base + 0x02;
            unsigned char bright_base = base + 0x10;

            if(mode == HYPERX_MODE_DIRECT)
            {
                bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_MODE_INDEPENDENT, HYPERX_MODE3_DIRECT);
            }

            for(int led = 0; led < 5; led++)
            {
                bus->i2c_smbus_write_byte_data(dev, red_base    + (3 * led), red  );
                bus->i2c_smbus_write_byte_data(dev, green_base  + (3 * led), green);
                bus->i2c_smbus_write_byte_data(dev, blue_base   + (3 * led), blue );
                bus->i2c_smbus_write_byte_data(dev, bright_base + (3 * led), 0x64 );
            }
        }
    }

    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x02);
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x03);
}

void HyperXController::SetLEDColor(unsigned int led, unsigned char red, unsigned char green, unsigned char blue)
{
    /*-----------------------------------------------------*\
    | led_slot - the unmapped slot ID for the given LED     |
    | led - the LED ID within that slot                     |
    | slot_id - counts enabled slots                        |
    | slot - the mapped slot ID for the given LED           |
    \*-----------------------------------------------------*/
    int led_slot    = led / 5;
    int slot_id     = -1;
    int slot;

    led            -= (led_slot * 5);

    /*-----------------------------------------------------*\
    | Loop through all possible slots and only count those  |
    | which are active.                                     |
    \*-----------------------------------------------------*/
    for(slot = 0; slot < 4; slot++)
    {
        if((slots_valid & ( 1 << slot)) != 0)
        {
            slot_id++;
        }

        if(slot_id == led_slot)
        {
            break;
        }
    }

    unsigned char base        = slot_base[slot];
    unsigned char red_base    = base + 0x00;
    unsigned char green_base  = base + 0x01;
    unsigned char blue_base   = base + 0x02;
    unsigned char bright_base = base + 0x10;

    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x01);

    bus->i2c_smbus_write_byte_data(dev, red_base    + (3 * led), red  );
    bus->i2c_smbus_write_byte_data(dev, green_base  + (3 * led), green);
    bus->i2c_smbus_write_byte_data(dev, blue_base   + (3 * led), blue );
    bus->i2c_smbus_write_byte_data(dev, bright_base + (3 * led), 0x64 );

    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x02);
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x03);
}


void HyperXController::SetLEDColor(unsigned int slot, unsigned int led, unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned char base        = slot_base[slot];
    unsigned char red_base    = base + 0x00;
    unsigned char green_base  = base + 0x01;
    unsigned char blue_base   = base + 0x02;
    unsigned char bright_base = base + 0x10;

    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x01);

    bus->i2c_smbus_write_byte_data(dev, red_base    + (3 * led), red  );
    bus->i2c_smbus_write_byte_data(dev, green_base  + (3 * led), green);
    bus->i2c_smbus_write_byte_data(dev, blue_base   + (3 * led), blue );
    bus->i2c_smbus_write_byte_data(dev, bright_base + (3 * led), 0x64 );

    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x02);
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x03);
}

void HyperXController::SetMode(unsigned char new_mode, bool random, unsigned short new_speed)
{
    mode  = new_mode;
    speed = new_speed;
    
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x01);

    /*-----------------------------------------------------*\
    | Determine which mode register to use.                 |
    | If set to random color mode, use Mode1.               |
    | If set to fixed color mode, use Mode2.                |
    \*-----------------------------------------------------*/
    unsigned char mode_reg;

    if(random)
    {
        mode_reg = HYPERX_REG_MODE_RANDOM;
    }
    else
    {
        mode_reg = HYPERX_REG_MODE_CUSTOM;
    }
    
    switch (mode)
    {
    case HYPERX_MODE_DIRECT:
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_MODE_INDEPENDENT, HYPERX_MODE3_DIRECT);
        break;

    case HYPERX_MODE_STATIC:
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_MODE_CUSTOM, HYPERX_MODE2_STATIC);
        break;

    case HYPERX_MODE_RAINBOW:
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_MODE_RANDOM, HYPERX_MODE1_RAINBOW);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_TIMER_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_TIMER_LSB, speed & 0xFF);
        break;

    case HYPERX_MODE_COMET:
        bus->i2c_smbus_write_byte_data(dev, mode_reg, HYPERX_MODE2_COMET);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_TIMER_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_TIMER_LSB, speed & 0xFF);
        break;

    case HYPERX_MODE_HEARTBEAT:
        bus->i2c_smbus_write_byte_data(dev, mode_reg, HYPERX_MODE2_HEARTBEAT);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_OFF_TIME_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_OFF_TIME_LSB, speed & 0xFF);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_ON_TIME_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_ON_TIME_LSB, speed & 0xFF);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_DELAY_TIME_MSB, 0x03);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_DELAY_TIME_LSB, 0xE8);
        break;

    case HYPERX_MODE_CYCLE:
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_MODE_RANDOM, HYPERX_MODE1_CYCLE);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_ON_TIME_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_ON_TIME_LSB, speed & 0xFF);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_CHANGE_TIME_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_CHANGE_TIME_LSB, speed & 0xFF);
        break;

    case HYPERX_MODE_BREATHING:
        bus->i2c_smbus_write_byte_data(dev, mode_reg, HYPERX_MODE2_BREATHING);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_FADE_IN_TIME_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_FADE_IN_TIME_LSB, speed & 0xFF);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_FADE_OUT_TIME_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_FADE_OUT_TIME_LSB, speed & 0xFF);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_OFF_TIME_MSB, 0x00);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_OFF_TIME_LSB, 0x00);
        break;

    case HYPERX_MODE_BOUNCE:
        bus->i2c_smbus_write_byte_data(dev, mode_reg, HYPERX_MODE2_BOUNCE);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_TIMER_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_TIMER_LSB, speed & 0xFF);
        break;

    case HYPERX_MODE_BLINK:
        bus->i2c_smbus_write_byte_data(dev, mode_reg, HYPERX_MODE2_BLINK);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_OFF_TIME_MSB, speed >> 8);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_OFF_TIME_LSB, speed & 0xFF);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_ON_TIME_MSB, 0x07);
        bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_ON_TIME_LSB, 0xD4);
        break;
    }

    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x02);
    bus->i2c_smbus_write_byte_data(dev, HYPERX_REG_APPLY, 0x03);
}
