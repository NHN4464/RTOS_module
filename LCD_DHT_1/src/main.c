#include "dht_display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stddef.h>

void app_main(void) {
    xTaskCreate(vDisplay, "LCD Task", 2048, NULL, 1, NULL);
}
