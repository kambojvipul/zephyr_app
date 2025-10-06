#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>

#include "app_config.h"
#include "app_events.h"
LOG_MODULE_REGISTER(adc, LOG_LEVEL_INF);


    uint16_t buf;
    uint16_t val_mv;
    int32_t vref_mv;
    uint8_t adc_state = 0; //0: not start, 1:inprogress, 2: error  

static const struct device *adc = DEVICE_DT_GET(DT_ALIAS(my_adc));
static const struct adc_channel_cfg adc_ch = ADC_CHANNEL_CFG_DT(MY_ADC_CH);



   //buffer and option for adc read
    struct adc_sequence seq = {
       .channels = BIT(adc_ch.channel_id),
       .buffer = &buf,
       .buffer_size = sizeof(buf),
       .resolution = DT_PROP(MY_ADC_CH, zephyr_resolution),
    };


//adc_read handler

void adc_read_handler(struct k_work *work)
{
    
    int ret = 0;
            struct k_work_delayable *dwork = k_work_delayable_from_work(work);

    adc_state = 1;
    ret = adc_read(adc, &seq);
        if (ret < 0) {
            printk("Could not read ADC: %d\r\n", ret);
            adc_state = 2;
        }
        

        // Calculate ADC value (mV)
        val_mv = buf * vref_mv / (1 << seq.resolution);

 

        // Print ADC value
        LOG_INF("ADC Done: Raw: %u, mV: %u\r\n", buf, val_mv);
        k_work_schedule(dwork, K_MSEC(sample_interval_ms));
}


int adc_init(void)
{
    int ret = 0;
    //adc init
    vref_mv = DT_PROP(MY_ADC_CH, zephyr_vref_mv);

    //check if adc device is ready
    if(!device_is_ready(adc)) {
        LOG_ERR("ADC device not ready\r\n");
        return 0;
    }

    //configure adc channel
    ret = adc_channel_setup(adc, &adc_ch);
    if(ret <0) {
        LOG_ERR("ADC channel setup error: %d\r\n", ret);
        return ret;
    }
    return 0;
    //adc init end
}
