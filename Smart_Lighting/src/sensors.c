#include <zephyr.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/printk.h>
#include "config.h"
#include "tb_pubsub.h"

#define BTN_PORT SW0_GPIO_NAME
#define BTN_COUNT 4

const u32_t btn_arr[BTN_COUNT] = {
    SW0_GPIO_PIN,
    SW1_GPIO_PIN,
    SW2_GPIO_PIN,
    SW3_GPIO_PIN
};

struct device *btn_dev;
struct gpio_callback btn_callback;

u32_t state[4];

int btn_alert_handler(struct k_alert *alert);
K_ALERT_DEFINE(btn_alert, btn_alert_handler, 10);

void btn_handler(struct device *port, struct gpio_callback *cb,
					u32_t pins)
{
    /* Context: interrupt handler */

	printk("Signalling alert!\n");
	k_alert_send(&btn_alert);
}

int btn_alert_handler(struct k_alert *alert)
{
	int value;
	char payload[25];


    /* Context: Zephy kernel workqueue thread */

	printk("Button event!\n");
	for (u32_t i = 0; i < BTN_COUNT; i++) {
		gpio_pin_read(btn_dev, btn_arr[i], &value);
		if (value != state[i]) {
            /* Formulate JSON in the format expected by thingsboard.io */
			// Check if Button press is Btn 1 --> Btn1 represents Door Sensor
			if(btn_arr[i]==btn_arr[0])
			{
			snprintf(payload, sizeof(payload), "{\"Door\":%s}",value == 0 ? "Open" : "Close");
			tb_publish_telemetry(payload);
			state[i] = value;
				// If Open switch on LED1 otherwise no light for Close
				if(value==0)
				{
					putLights(0,1);
				}
				else
				{
					putLights(0,0);
				}
		
			}
			//Check if Button press Btn2 --> Btn2 represents Student Sensor which checks presence of Students in class
			// By default Student is always present in the class; we are taking this scenario
			if(btn_arr[i]==btn_arr[1])
			{
			snprintf(payload, sizeof(payload), "{\"Student\":%s}", value == 0 ? "Absent" : "Present");
			tb_publish_telemetry(payload);
			state[i] = value;
			//If Present Put on the Light LED2 othwerwise no
			if(value==0)
				{
					putLights(1,0);
				}
				else
				{
					putLights(1,1);
				}
			}
			//Check if Button press Btn3 --> Btn3 represents Projector Sensor which checks Projector is on or off
			if(btn_arr[i]==btn_arr[2])
			{
			snprintf(payload, sizeof(payload), "{\"Projector\":%s}", value == 0 ? "On" : "Off");
			tb_publish_telemetry(payload);
			state[i] = value;
			//If projector is On then switch LED3 on otherwise Off
			if(value==0)
				{
					putLights(2,1);
				}
				else
				{
					putLights(2,0);
				}
			}
			//Check if Button4 --> Bt3 represents Board sensor,if lecture is writing or not
			if(btn_arr[i]==btn_arr[3])
			{
			snprintf(payload, sizeof(payload), "{\"Board\":%s}",value == 0 ? "Writing" : "NoWrite");
			tb_publish_telemetry(payload);
			state[i] = value;
			//If writing switch the light on
			if(value==0)
				{
					putLights(3,1);
				}
				else
				{
					putLights(3,0);
				}
			}
		}
	}



		
	return 0;



}

void sensors_start()
{
	btn_dev = device_get_binding(BTN_PORT);

	for (u32_t i = 0; i < BTN_COUNT; i++) {
		gpio_pin_configure(btn_dev, btn_arr[i], GPIO_DIR_IN | GPIO_INT
			| GPIO_INT_DOUBLE_EDGE | GPIO_INT_EDGE | GPIO_PUD_PULL_UP);
	}

	gpio_init_callback(&btn_callback, btn_handler,
		BIT(btn_arr[0]) | BIT(btn_arr[1]) |
		BIT(btn_arr[2]) | BIT(btn_arr[3])
	);

	gpio_add_callback(btn_dev, &btn_callback);

	for (u32_t i = 0; i < BTN_COUNT; i++) {
		gpio_pin_enable_callback(btn_dev, btn_arr[i]);
		gpio_pin_read(btn_dev, btn_arr[i], &state[i]);
	}
}
