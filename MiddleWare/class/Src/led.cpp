#include "led.hpp"

Led::Led(GPIO_TypeDef *port, uint16_t pin)
{
    this->port   = port;
    this->pin    = pin;
    this->status = false;
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

void Led::init(GPIO_TypeDef *port, uint16_t pin)
{
    this->port   = port;
    this->pin    = pin;
    this->status = false;
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

void Led::on(void)
{
    this->status = true;
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

void Led::off(void)
{
    this->status = false;
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

void Led::toggle(void)
{
    if (this->status) {
        this->off();
    } else {
        this->on();
    }
}
