#ifndef MUSIC_H
#define MUSIC_H

void ledc_init(void);
void play_tone(int freq, int duration_ms);
void music_task(void *pvParameters);

#endif // MUSIC_H
