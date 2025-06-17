#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#define LED_PIN             GPIO_NUM_16
#define BUTTON_PIN          GPIO_NUM_21  // Nút để thêm giá trị vào buffer
#define LED_ON_BUTTON_PIN   GPIO_NUM_22  // Nút để bật LED
#define LED_OFF_BUTTON_PIN  GPIO_NUM_23  // Nút để tắt LED
#define BUF_SIZE            5

static int buf[BUF_SIZE];
static int head = 0;
static int tail = 0;
static int led_mode = 0;  // 0: OFF, 1: BLINK, 2: ON

static SemaphoreHandle_t sem_filled;

void producer(void *parameters) {
    int last_button_state = 1;
    while (1) {
        int current_state = gpio_get_level(BUTTON_PIN);

        if (last_button_state == 1 && current_state == 0) {
            if (led_mode == 2) {
                led_mode = 1;  // Nếu đang sáng liên tục, chuyển sang chế độ nhấp nháy
                printf("BTN pressed → LED switched to BLINK mode\n");
            } else if (led_mode == 1) {
                buf[head] = 1;
                head = (head + 1) % BUF_SIZE;
                xSemaphoreGive(sem_filled);
                printf("BTN pressed → Triggered blinking\n");
            } else {
                printf("BTN pressed → LED is OFF, nothing to do\n");
            }
        }

        last_button_state = current_state;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void consumer(void *parameters) {
    while (1) {
        xSemaphoreTake(sem_filled, portMAX_DELAY);
        int val = buf[tail];
        buf[tail] = 0;

        if (val && led_mode == 1) {
            printf("Blinking 3 times from index %d\n", tail);
            for (int i = 0; i < 3; i++) {
                if (led_mode != 1) {
                    gpio_set_level(LED_PIN, (led_mode == 2) ? 1 : 0);
                    printf("Blink interrupted at %d/3 due to mode change\n", i + 1);
                    break;
                }

                gpio_set_level(LED_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(300));
                gpio_set_level(LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(300));

                printf("Blink %d/3 done\n", i + 1);
            }
        } else {
            printf("Blink skipped (mode: %d)\n", led_mode);
        }

        // Sau khi blink xong, giữ LED đúng trạng thái theo mode
        gpio_set_level(LED_PIN, (led_mode == 2) ? 1 : 0);

        tail = (tail + 1) % BUF_SIZE;
    }
}

void control_led(void *parameters) {
    int last_on_state = 1;
    int last_off_state = 1;

    while (1) {
        int current_on = gpio_get_level(LED_ON_BUTTON_PIN);
        int current_off = gpio_get_level(LED_OFF_BUTTON_PIN);

        if (last_on_state == 1 && current_on == 0) {
            led_mode = 2;
            gpio_set_level(LED_PIN, 1);
            printf("LED turned ON (mode: ON)\n");
        }

        if (last_off_state == 1 && current_off == 0) {
            led_mode = 0;
            gpio_set_level(LED_PIN, 0);
            printf("LED turned OFF\n");
        }

        last_on_state = current_on;
        last_off_state = current_off;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}


void app_main(void) {
    // Cấu hình LED
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Cấu hình các nút: BUTTON_PIN, LED_ON_BUTTON_PIN, LED_OFF_BUTTON_PIN
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;

    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = (1ULL << LED_ON_BUTTON_PIN);
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = (1ULL << LED_OFF_BUTTON_PIN);
    gpio_config(&io_conf);

    // Semaphore counting
    sem_filled = xSemaphoreCreateCounting(BUF_SIZE, 0);

    // Tạo các task
    xTaskCreate(producer, "Producer Task", 2048, NULL, 1, NULL);
    xTaskCreate(consumer, "Consumer Task", 2048, NULL, 1, NULL);
    xTaskCreate(control_led, "Control LED Task", 2048, NULL, 2, NULL);
}
