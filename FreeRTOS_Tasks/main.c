#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define led1 2
#define ledR 33
#define ledG 26
#define ledB 14
#define delayR 1000
#define delayG 100
#define delayB 4000
#define STACK_SIZE 1024 * 2

static const char *TAG = "Main";
int led_level = 0;

esp_err_t init_led(void);
esp_err_t create_tasks(void);
void vTaskR(void *pvParameters);
void vTaskG(void *pvParameters);
void vTaskB(void *pvParameters);

void app_main(void)
{
    init_led();
    create_tasks();
    printf("Number of cores : %u\n", portNUM_PROCESSORS);

    while (1)
    {   
        gpio_set_level(led1, led_level);
        led_level = !led_level;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

esp_err_t init_led(void)
{
    gpio_reset_pin(led1);
    gpio_reset_pin(ledR);
    gpio_reset_pin(ledG);
    gpio_reset_pin(ledB);

    gpio_set_direction(led1, GPIO_MODE_OUTPUT);
    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);
    gpio_set_direction(ledB, GPIO_MODE_OUTPUT);

    return ESP_OK;
}

esp_err_t create_tasks(void)
{
    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;

    // Create the task, storing the handle.  Note that the passed parameter ucParameterToPass
    // must exist for the lifetime of the task, so in this case is declared static.  If it was just an
    // an automatic stack variable it might no longer exist, or at least have been corrupted, by the time
    // the new task attempts to access it.

    xTaskCreatePinnedToCore(vTaskR,
                "NAME",
                STACK_SIZE,
                &ucParameterToPass,
                1,
                &xHandle,
                0);

    xTaskCreatePinnedToCore(vTaskG,
                "NAME",
                STACK_SIZE,
                &ucParameterToPass,
                1,
                &xHandle,
                1);

    xTaskCreatePinnedToCore(vTaskB,
                "NAME",
                STACK_SIZE,
                &ucParameterToPass,
                1,
                &xHandle,
                tskNO_AFFINITY);

    return ESP_OK;
}

void vTaskR(void *pvParameters)
{
    while (1)
    {
        ESP_LOGE(TAG, "LED R Core 0");
        gpio_set_level(ledR, 1);
        vTaskDelay(pdMS_TO_TICKS(delayR));
        gpio_set_level(ledR, 0);
        vTaskDelay(pdMS_TO_TICKS(delayR));        
    }
}

void vTaskG(void *pvParameters)
{
    while (1)
    {
        ESP_LOGI(TAG, "LED G Core 1");
        gpio_set_level(ledG, 1);
        vTaskDelay(pdMS_TO_TICKS(delayG));
        gpio_set_level(ledG, 0);
        vTaskDelay(pdMS_TO_TICKS(delayG));          
    }
}

void vTaskB(void *pvParameters)
{
    while (1)
    {
        ESP_LOGW(TAG, "LED B Core ANY");
        gpio_set_level(ledB, 1);
        vTaskDelay(pdMS_TO_TICKS(delayB));
        gpio_set_level(ledB, 0);
        vTaskDelay(pdMS_TO_TICKS(delayB));  
    }
}
