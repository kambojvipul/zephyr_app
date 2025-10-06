#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#include "app_config.h"
#include "app_events.h"

int sample_interval_ms;
int voltage_threshold_mv;
bool enable_ble;


K_WORK_DELAYABLE_DEFINE(adc_read_work, adc_read_handler);
K_WORK_DELAYABLE_DEFINE(ble_notify_work, ble_notify_handler);


K_THREAD_DEFINE(led_rtos_thread_id, STACKSIZE, led_rtos_thread, NULL, NULL, NULL, 1, 0,0);

int main(void)
{
    //int ret = 0;

    // reading from Custom Kconfig 
    int SAMPLE_INTERVAL_MS = CONFIG_APP_SAMPLE_INTERVAL_MS;
    LOG_INF("Kconfig: Sample Interval: %d ms\r\n", SAMPLE_INTERVAL_MS);
    bool ble_enable = IS_ENABLED(CONFIG_APP_ENABLE_BLE);
    LOG_INF("Kconfig: BLE Enable: %d\r\n", ble_enable);
    bool led_active_low = IS_ENABLED(CONFIG_APP_LED_ACTIVE_LOW);
    LOG_INF("Kconfig: LED Active Low: %d\r\n", led_active_low);

    //reading from app device tree node
    if (!DT_NODE_HAS_STATUS(APP_NODE, okay)) {
        LOG_ERR("Error: 'app,config' node not found or not okay!\n");
        return 0;
    }
     sample_interval_ms = DT_PROP(APP_NODE, sample_interval_ms);
     voltage_threshold_mv = DT_PROP(APP_NODE, voltage_threshold_mv);
     enable_ble = DT_NODE_HAS_PROP(APP_NODE, enable_ble);

    LOG_INF("App Node: Sample interval: %d ms\n", sample_interval_ms);
    LOG_INF("App Node: Voltage threshold: %d mV\n", voltage_threshold_mv);
    LOG_INF("App Node: BLE enabled: %s\n", enable_ble ? "yes" : "no");


    adc_init();
    if(enable_ble)
    ble_init();
    button_init();
    led_init();
    
    //schedule the threads
    k_work_schedule(&adc_read_work, K_MSEC(sample_interval_ms));
    if(enable_ble)
    k_work_schedule(&ble_notify_work, K_MSEC(sample_interval_ms));

    while(1)
    {
        k_msleep(5000);
    }

    return 0;
}