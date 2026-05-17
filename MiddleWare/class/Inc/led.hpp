#pragma once

extern "C" {
#include "stm32f1xx_hal.h"
}

class Led {
public:
    Led(GPIO_TypeDef *port, uint16_t pin);
    void init(GPIO_TypeDef *port, uint16_t pin);
    void on(void);
    void off(void);
    void toggle(void);

private:
    GPIO_TypeDef *port;
    uint16_t      pin;
    bool          status;   // true = 亮, false = 灭
};
