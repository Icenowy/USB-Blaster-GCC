/*
led.c - led driver

MIT License

Copyright (c) 2016 Yongqian Tang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "base.h"
#include "timebase.h"
#include "led.h"

// LEDģʽö��
typedef enum {
    LED_MODE_NORMAL,    // ��ͨģʽ����/��
    LED_MODE_FLASH,     // ��˸ģʽ

    LED_MODE_MAX
} led_mode_e;

// LEDģʽ
static led_mode_e s_led_mode;

// �ϴθ���ʱ��(ms)
static uint32_t s_led_last_time;

// LED��ʱ(ms)
static uint16_t s_led_timer;

// LED��˸����(ms)
static uint16_t s_led_period;

// LEDÿ��˸���ڵ���ʱ��(ms)
static uint16_t s_led_pulse;

// LED��˸����
static uint16_t s_led_cycle_cnt;

/*-----------------------------------*/

// ��/��LED
#if defined(BLUEPILL)
#define LED_ON()        GPIOC->BSRR = GPIO_Pin_13
#define LED_OFF()       GPIOC->BRR = GPIO_Pin_13
#elif defined(JTAGPILL)
#define LED_ON()	GPIOB->BSRR = GPIO_Pin_8
#define LED_OFF()	GPIOB->BRR = GPIO_Pin_8
#elif defined(STLINK_V2_CLONE_DONGLE)
#define LED_ON()	GPIOA->BSRR = GPIO_Pin_9
#define LED_OFF()	GPIOA->BRR = GPIO_Pin_9
#endif

// LED�˿ڳ�ʼ��
static void led_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

#if defined(BLUEPILL)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // GPIOA Configuration: Pin 13
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
#elif defined(STLINK_V2_CLONE_DONGLE)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // GPIOA Configuration: Pin 9
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

/*-----------------------------------*/

// LED��ʼ��
void led_init(void)
{
    led_gpio_config();
    LED_OFF();

    s_led_last_time = millis();
    s_led_mode = LED_MODE_NORMAL;
}

// LED����
void led_light(BOOL is_light)
{
    s_led_mode = LED_MODE_NORMAL;
    if (is_light) {
        LED_ON();
    } else {
        LED_OFF();
    }
}

// LED��˸
// ����������(ms)������ʱ��(ms)����˸����(=0ʱһֱ��˸)
void led_flash(uint16_t period, uint16_t pulse, uint16_t cycles)
{
    s_led_mode = LED_MODE_FLASH;
    s_led_period = period;
    s_led_pulse = pulse;
    s_led_cycle_cnt = cycles;
    s_led_timer = 0;
    s_led_last_time = millis();
    LED_OFF();
}

/*-----------------------------------*/

// LED��ʱ����
void led_update(void)
{
    uint32_t t;

    switch (s_led_mode) {
    case LED_MODE_FLASH:
        t = millis();
        s_led_timer += (t - s_led_last_time);
        s_led_last_time = t;
        if (s_led_timer >= s_led_pulse) {
            LED_OFF();
        }
        if (s_led_timer >= s_led_period) {
            if (s_led_cycle_cnt > 0) {
                s_led_cycle_cnt--;
                if (s_led_cycle_cnt == 0) {
                    s_led_mode = LED_MODE_NORMAL;
                    LED_OFF();
                    return;
                }
            }
            s_led_timer = 0;
            LED_ON();
        }
        break;
    default:
        break;
    }
}
