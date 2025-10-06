#ifndef APP_UUIDS_H
#define APP_UUIDS_H

#include <zephyr/bluetooth/uuid.h>

/* Custom Service Variables */
#define BT_UUID_CUSTOM_SERVICE_VAL \
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)


#endif