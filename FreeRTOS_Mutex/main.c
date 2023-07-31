#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define STACK_SIZE 1024 * 2
#define ledR 33
#define ledG 26
#define delayR 1000
#define delayG 2000

xSemaphoreHandle GlobalKey = 0;
static const char *TAG = "Main";
int led_level = 0;

esp_err_t init_led(void);
esp_err_t create_tasks(void);
esp_err_t shared_resource(int led);
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

void app_main(void)
{
    init_led();
    create_tasks();
    GlobalKey = xSemaphoreCreateMutex();

}

esp_err_t init_led(void)
{
    gpio_reset_pin(ledR);
    gpio_reset_pin(ledG);

    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);

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

    xTaskCreatePinnedToCore(vTask1,
                            "NAME",
                            STACK_SIZE,
                            &ucParameterToPass,
                            1,
                            &xHandle,
                            0);

    xTaskCreatePinnedToCore(vTask2,
                            "NAME",
                            STACK_SIZE,
                            &ucParameterToPass,
                            1,
                            &xHandle,
                            1);

    return ESP_OK;
}


void vTask1(void *pvParameters)
{
    while (1)
    {
        if(xSemaphoreTake(GlobalKey, pdMS_TO_TICKS(100))){
            ESP_LOGE(TAG, "Task R took the resource");
            shared_resource(ledR);
            xSemaphoreGive(GlobalKey);
        }

        vTaskDelay(pdMS_TO_TICKS(delayR));
    }
}

void vTask2(void *pvParameters)
{
    while (1)
    {
        if(xSemaphoreTake(GlobalKey, pdMS_TO_TICKS(100))){
            ESP_LOGI(TAG, "Task G took the resource");
            shared_resource(ledG);
            xSemaphoreGive(GlobalKey);
        }

        vTaskDelay(pdMS_TO_TICKS(delayG)); 
    }
}

esp_err_t shared_resource(gpio_num_t led)
{
    for(size_t i = 0; i < 8; ++i){
        vTaskDelay(pdMS_TO_TICKS(400));
        gpio_set_level(led, 1);
        vTaskDelay(pdMS_TO_TICKS(400));
        gpio_set_level(led, 0);
    }

    return ESP_OK;
}
