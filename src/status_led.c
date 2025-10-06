#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#include "app_config.h"
#include "app_events.h"
LOG_MODULE_REGISTER(led, LOG_LEVEL_INF);

 uint16_t led_time = 100;
 int led_state = 0;

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);
static const struct pwm_dt_spec pwm_led = PWM_DT_SPEC_GET(DT_NODELABEL(pwm_led));

int led_init(void)
{
    //led init
    int ret = 0;
    ret = gpio_is_ready_dt(&led);
    if(ret <0)
    {
        
        LOG_ERR("Error: LED device is not ready : %d\n",ret);
        return ret;
    }
    
    if (!device_is_ready(pwm_led.dev)) 
    {
	    LOG_ERR("Error: PWM Led device %s is not ready\n", pwm_led.dev->name);
	    return 0;
    }
    
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
    if(ret <0)
        return ret;
    
    
    ret = pwm_set_pulse_dt(&pwm_led, 1000);
    if (ret) 
    {
    LOG_ERR("Error in pwm_set_dt(), err: %d", ret);
    }

    return 0;
}


void led_rtos_thread(void *p1, void *p2, void *p3)
{
    while(1)
    {
      switch(adc_state)
      {
        case 0:
                gpio_pin_set_dt(&led, 0);
                pwm_set_pulse_dt(&pwm_led, 0);
                k_msleep(500);
                gpio_pin_set_dt(&led, 1);
                pwm_set_pulse_dt(&pwm_led, 1500);
                k_msleep(500);
            break;

        case 1:
                gpio_pin_set_dt(&led, 0);
                pwm_set_pulse_dt(&pwm_led, 0);
                k_msleep(200);
                gpio_pin_set_dt(&led, 1);
                pwm_set_pulse_dt(&pwm_led, 1500);
                k_msleep(50);
                gpio_pin_set_dt(&led, 0);
                pwm_set_pulse_dt(&pwm_led, 0);
                k_msleep(50);
                gpio_pin_set_dt(&led, 1);
                pwm_set_pulse_dt(&pwm_led, 1500);
                k_msleep(50);
                gpio_pin_set_dt(&led, 0);
                pwm_set_pulse_dt(&pwm_led, 0);
                k_msleep(500);
                adc_state = 0;
            break;
        case 2:
   
            gpio_pin_set_dt(&led, 0);
            pwm_set_pulse_dt(&pwm_led, 0);
            k_msleep(50);
            gpio_pin_set_dt(&led, 1);
            pwm_set_pulse_dt(&pwm_led, 1500);
            k_msleep(50);
            break;

        default: 
            k_msleep(50);     
      }     
    }
}