#pragma once

#include <string>
#include "stm32f1xx_hal.h"
#include "i2c.hpp"

class oled
{
public:
    oled(GPIO_TypeDef* port, uint16_t scl, uint16_t sda);
    int get_id(void);
    int show_num(int num, int x, int y);
    int show_string(std::string str, int x, int y);
    int set_pixel(unsigned char x, unsigned char y);
    int clear_pixel(unsigned char x, unsigned char y);
    int draw_wave(const uint16_t *data, unsigned char len);
    int refresh(void);
    int clear(void);
private:
    unsigned char buffer[1024];
    soft_i2c i2c;
    int write_cmd(unsigned char cmd);
    int write_data(unsigned char data);
    int setcursor(unsigned char x, unsigned char y);
};