#include <zephyr.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include "lights.h"

#define LED_PORT LED0_GPIO_PORT
#define LED_COUNT 4

const u32_t led_arr[LED_COUNT] = {
    LED0_GPIO_PIN,
    LED1_GPIO_PIN,
    LED2_GPIO_PIN,
    LED3_GPIO_PIN
};

struct device *led_dev;

void putLights(u32_t ledno, bool state)
{
    gpio_pin_write(led_dev, led_arr[ledno], state ? 0 : 1);
}

bool getLights(u32_t ledno)
{
    u32_t value;

    gpio_pin_read(led_dev, led_arr[ledno], &value);

    return value ? false : true;
}

void lights_init()
{
    led_dev = device_get_binding(LED_PORT);

    for (u32_t i = 0; i < LED_COUNT; i++) {
        gpio_pin_configure(led_dev, led_arr[i], GPIO_DIR_OUT);
        gpio_pin_write(led_dev, led_arr[i], 1);
    }
}
