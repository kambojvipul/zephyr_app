#ifndef APP_EVENTS_H
#define APP_EVENTS_H


extern int sample_interval_ms;
extern int voltage_threshold_mv;
extern bool enable_ble;

extern int enable_notification;
extern uint8_t adc_state;


int adc_init(void);
int ble_init(void);
int button_init(void);
int led_init(void);

//work handelers
void led_blink_handler(struct k_work *work);
void adc_read_handler(struct k_work *work);
void ble_notify_handler(struct k_work *work);

//thread
extern void led_rtos_thread(void *p1, void *p2, void *p3);

#endif