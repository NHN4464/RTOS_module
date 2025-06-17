#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "music.h"

#define TAG "MAIN"

void app_main(void) {
    ESP_LOGI(TAG, "Initializing LEDC...");
    ledc_init();

    ESP_LOGI(TAG, "Starting music task...");
    xTaskCreate(music_task, "music_task", 2048, NULL, 5, NULL);
}
