#include "music.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pitches.h"

#define SPEAKER_PIN 18

// Giai điệu Happy Birthday
int melody[] = {
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
    NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4
};

int noteDurations[] = {
    4, 4, 4, 4, 4, 2,
    4, 4, 4, 4, 4, 2,
    4, 4, 4, 4, 4, 4, 2,
    4, 4, 4, 4, 4, 2
};

void ledc_init(void) {
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = SPEAKER_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = 0
    };
    ledc_channel_config(&channel_conf);
}

void play_tone(int freq, int duration_ms) {
    if (freq == 0) {
        vTaskDelay(pdMS_TO_TICKS(duration_ms));
        return;
    }

    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 128);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void music_task(void *pvParameters) {
    while (1) {
        int notesCount = sizeof(melody) / sizeof(melody[0]);
        for (int i = 0; i < notesCount; i++) {
            int noteDuration = 1000 / noteDurations[i];
            play_tone(melody[i], noteDuration);
            vTaskDelay(pdMS_TO_TICKS(noteDuration * 0.3));
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
