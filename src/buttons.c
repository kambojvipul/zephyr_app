#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "app_config.h"
#include "app_events.h"
LOG_MODULE_REGISTER(button, LOG_LEVEL_INF);


static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(my_button), gpios);


   int enable_notification = 1;
   int soft_debounce = 0;
   int btn_state;
   int64_t curr_time, pressed_time = 0;

   static struct gpio_callback button_cb_data;

   void button_isr(const struct device *dev,
                struct gpio_callback *cb,
                uint32_t pins)
{
    // Check if the correct button was pressed
    if (BIT(button.pin) & pins) {
        btn_state = gpio_pin_get_dt(&button);

        // SOFTWARE DEBOUNCE CODE BELOW
        if(btn_state)
        {
            curr_time = k_uptime_get(); //in ms
            if(curr_time - pressed_time >2000)
            {
                enable_notification = !enable_notification;
                
            LOG_INF("Button pressed! Pin: %d\r\n", button.pin);    
            if(enable_notification)
            LOG_INF("BLE Notofication ON.\n");
            else
            LOG_INF("BLE Notofication OFF.\n");

            pressed_time = k_uptime_get(); //in ms
            }
        }  
    }
}


 int button_init(void)
{
    int ret = 0;
    //button init
    if(!gpio_is_ready_dt(&button)) {
        return 0;
    }
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if(ret <0)
    {
        LOG_ERR("ERROR: Button Pin Init Error\n");
        return 0;
    }

    //set up button interrupt
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        LOG_ERR("ERROR: could not configure button as interrupt source\r\n");
        return 0;
    }

    // Connect callback function (ISR) to interrupt source
    gpio_init_callback(&button_cb_data, button_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    return 0;
}