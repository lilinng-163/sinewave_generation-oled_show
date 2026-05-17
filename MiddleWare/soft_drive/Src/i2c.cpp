#include <iostream>
#include <string>
#include <cstdio>
#include "i2c.hpp"
#include "stm32f1xx_hal.h"

// 微秒级延时（72MHz 下大约 12 个循环 ≈ 1μs）
static void delay_us(void)
{
    for(volatile int i = 0; i < 12; i++);
}

void soft_i2c::scl_write(int x)
{
    HAL_GPIO_WritePin(port, scl, (GPIO_PinState)x);
    delay_us();
}
void soft_i2c::sda_write(int x)
{
    HAL_GPIO_WritePin(port, sda, (GPIO_PinState)x);
    delay_us();
}
int soft_i2c::start(void)
{
    sda_write(1);
    scl_write(1);
    sda_write(0);
    scl_write(0);

    return 0;
}
int soft_i2c::stop(void)
{
    sda_write(0);
    scl_write(0);
    scl_write(1);
    sda_write(1);

    return 0;
}
int soft_i2c::send_ack(unsigned char ack)
{
    scl_write(0);
    if(ack)
        sda_write(1);
    else
        sda_write(0);
    scl_write(1);
    scl_write(0);

    return 0;
}
int soft_i2c::receive_ack(void)
{
    int ack;
    scl_write(0);
    sda_write(1);
    delay_us();
    scl_write(1);
    delay_us();
    ack = HAL_GPIO_ReadPin(port,sda);
    scl_write(0);
    return ack;
}
int soft_i2c::send_byte(uint8_t byte)
{
    scl_write(0);
    sda_write(0);
    for(auto i = 0; i < 8; i++)
    {
        if(byte & (0x80 >> i))
        {
            sda_write(1);
        }
        scl_write(1);
        scl_write(0);
        sda_write(0);
    }

    return 0;
}
unsigned char soft_i2c::receive_byte(void)
{
    unsigned char data = 0x00;
    sda_write(1);

    for(auto i = 0; i < 8 ; i++)
    {
        scl_write(1);
        if(HAL_GPIO_ReadPin(port, sda))
        {
            data |= (0x80 >> i);
        }
        scl_write(0);
    }
    return data;
}
int soft_i2c::send_data(uint8_t* data, int len)
{
    for(auto i = 0; i < len; i++)
    {
        send_byte(data[i]);
    }

    return 0;
}
unsigned char* soft_i2c::receive_data(unsigned char* data,int len)
{
    for(auto i = 0; i < len; i++)
    {
        data[i] = receive_byte();
    }
    return data;
}
soft_i2c::~soft_i2c(void)
{

}