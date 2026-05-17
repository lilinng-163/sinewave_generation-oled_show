#pragma once

#include "stm32f1xx_hal.h"

class soft_i2c
{
private:
    GPIO_TypeDef* port;
    uint16_t scl;
    uint16_t sda;
    void scl_write(int x);
    void sda_write(int x);
public:
    soft_i2c(GPIO_TypeDef* port, uint16_t scl, uint16_t sda) : port(port), scl(scl), sda(sda) 
    {
        // 重新配置 SCL/SDA 为开漏输出（I2C 必须开漏）
        GPIO_InitTypeDef gpio = {0};
        gpio.Pin = scl | sda;
        gpio.Mode = GPIO_MODE_OUTPUT_OD;
        gpio.Pull = GPIO_PULLUP;
        gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(port, &gpio);

        HAL_GPIO_WritePin(this->port, this->scl, GPIO_PIN_SET);
        HAL_GPIO_WritePin(this->port, this->sda, GPIO_PIN_SET);
    };
    ~soft_i2c(void);
    int send_data(uint8_t* data, int len);
    unsigned char* receive_data(unsigned char* data,int len);
    int start(void);
    int stop(void);
    int send_ack(unsigned char ack);
    int receive_ack(void);
    int send_byte(uint8_t byte);
    unsigned char receive_byte();
};