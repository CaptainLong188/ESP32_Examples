#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define led1 2
#define STACK_SIZE 1024 * 4

static const char *TAG = "Main";
QueueHandle_t xQueue;
int led_level = 0;

typedef struct
{
    char id;
    char data[20];
} message_t;

esp_err_t init_led(void);
esp_err_t create_tasks(void);
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

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
    gpio_set_direction(led1, GPIO_MODE_OUTPUT);

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

// Task to create a Queue and post a value

void vTask1(void *pvParameters)
{
    message_t *messageExample;
    xQueue = xQueueCreate(10, sizeof(message_t *));
    char ID[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '\n'};

    // Create a queue capable of containing 10 pointers to message_t structures.
    // These should be passed by pointer as they contain a lot of data.

    while (1)
    {
        if (xQueue == NULL)
        {
            ESP_LOGE(TAG, "Failed to create the queue");
        }

        for (size_t i = 0; i < 9; ++i)
        {
            messageExample->id = ID[i];
            strncpy(messageExample->data, "Hello Task2", sizeof(messageExample->data));

            if (!xQueueSend(xQueue, (void *)&messageExample, pdMS_TO_TICKS(100)))
            {
                ESP_LOGE(TAG, "Error sending data to queue");
            }

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

// Task to receive from the queue.

void vTask2(void *pvParameters)
{
    message_t *readMessageExample;

    while (1)
    {
        if(xQueue != NULL)
        {
            // Receive a message on the created queue.  Block for 1s if a
            // message is not immediately available.
            if (!xQueueReceive(xQueue, &readMessageExample, pdMS_TO_TICKS(1000)))
            {
                ESP_LOGE(TAG, "Error receiving value from queue");
            }
            else
            {
                // readMessageExample now points to the struct AMessage variable posted
                // by vTask1.

                char id = readMessageExample->id;
                char data[20];
                strncpy(data, readMessageExample->data, sizeof(data));

                ESP_LOGI(TAG, "ID : %c Data : %s", id, data);
            }
        }
    }
}
