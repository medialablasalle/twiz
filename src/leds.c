#include "leds.h"
#include "nrf_gpio.h"
#include "high_res_timer.h"
#include "boards.h"

void led_on(int c)
{
    if (is_new_rev()) {
        // inverted logic
        nrf_gpio_pin_clear(c);
    } else {
        if (c == LED_B)
            c = LED_R; // there's no blue in old rev
        nrf_gpio_pin_set(c);
    }
}

void led_off(int c)
{
    if (is_new_rev()) {
        // inverted logic
        nrf_gpio_pin_set(c);
    } else {
        if (c == LED_B)
            c = LED_R; // there's no blue in old rev
        nrf_gpio_pin_clear(c);
    }
}

void led_write(int c, uint8_t state) {
    if (state)
        led_on(c);
    else
        led_off(c);
}

void leds_init(void)
{
    /* Detect if blue LED is available using its common anode pull up.
       There are 2 kinds of Twiz, inverted logic LEDs or not.
       if we can detect the pullup then this is the new revision, with blue LED ;)
    */
    nrf_gpio_cfg_output(LED_2);
    nrf_gpio_pin_set(LED_2);

    nrf_gpio_cfg_input(LED_2, GPIO_PIN_CNF_PULL_Pulldown);
    if (nrf_gpio_pin_read(LED_2))
        set_rev(NEW_REV);

    for (int i = LED_0; i <= LED_2; i++) {
        nrf_gpio_cfg_output(i);
        led_off(i);
    }
}

void led_blink(int c, uint16_t ms)
{
    led_on(c);
    nrf_timer_delay_ms(ms);
    led_off(c);
    nrf_timer_delay_ms(ms);
}

