#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <math.h>
#include "stm32f1xx_hal.h"
#include "App_task.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "led.hpp"
#include "oled.hpp"

extern uint16_t adc_buf[128];

static uint32_t     led_task_stack_size = 128;
static UBaseType_t  led_task_priority   = 1;
static TaskHandle_t led_task_handle;

int led_task(void *pcParameters)
{
    Led led(GPIOB, GPIO_PIN_6);
    
    while (1) 
    {
        led.toggle();
        vTaskDelay(1000);
    }
    return 0;
}

static uint32_t uart_task_stack_size = 256;
static UBaseType_t uart_task_priority = 2;
static TaskHandle_t uart_task_handle;

int uart_task(void *pvParamters)
{
    using namespace std;

    while(1)
    {
        UBaseType_t free = uxTaskGetStackHighWaterMark(NULL);
        printf("uart_task free: %d\r\n",(int)free);
        vTaskDelay(1000);
    }
    return 0;
}
static uint32_t     oled_task_stack_size  = 2048;
static UBaseType_t  oled_task_priority    = 2;
static TaskHandle_t oled_task_handle;
int oled_task(void *pvParamters)
{
    oled my_oled(GPIOB, GPIO_PIN_8, GPIO_PIN_9);
    uint16_t local_buf[128];

    while(1)
    {
        // 先拍快照，避免 DMA 实时写入导致数据混乱
        memcpy(local_buf, adc_buf, sizeof(local_buf));

        my_oled.clear();
        my_oled.draw_wave(local_buf, 128);
        my_oled.refresh();
        vTaskDelay(200);
    }
    return 0;
}
static uint32_t     start_task_stack_size = 512;
static UBaseType_t  start_task_priority   = 1;
static TaskHandle_t start_task_handle;

int start_task(void *pvParameters)
{
    taskENTER_CRITICAL();
    xTaskCreate((TaskFunction_t)led_task,
                (char *)"led_task",
                led_task_stack_size,
                (void *)NULL,
                led_task_priority,
                &led_task_handle);

    xTaskCreate((TaskFunction_t)uart_task,
                (char *)"uart_task",
                uart_task_stack_size,
                (void *)NULL,
                uart_task_priority,
                &uart_task_handle);
    xTaskCreate((TaskFunction_t)oled_task,
                (char *)"oled_task",
                oled_task_stack_size,
                (void *)NULL,
                oled_task_priority,
                &oled_task_handle
                );
    taskEXIT_CRITICAL();
    vTaskDelete(NULL);

    return 0;
}

int start_freertos(void)
{
    xTaskCreate((TaskFunction_t)start_task,
                (char *)"start_task",
                start_task_stack_size,
                (void *)NULL,
                start_task_priority,
                &start_task_handle);
    vTaskStartScheduler();
    return 0;
}
