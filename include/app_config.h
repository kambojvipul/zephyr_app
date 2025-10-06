#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>

//stacksize and delay
#define STACKSIZE 512

//custom app node
#define APP_NODE DT_PATH(app)


// Get ADC device and channel configuration from devicetree
#define MY_ADC_CH DT_ALIAS(my_adc_channel)


// Work Queues (defined in main.c, scheduled/used by others)
extern struct k_work_delayable led_blink_work;
extern struct k_work_delayable adc_read_work;
extern struct k_work_delayable ble_notify_work;

// Event handlers that need to be called by main
void ble_notify_handler(struct k_work *work);
void adc_read_handler(struct k_work *work);
void led_blink_handler(struct k_work *work);

#endif