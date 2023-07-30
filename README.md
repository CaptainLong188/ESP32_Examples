# ESP32_Examples

## FreeRTOS_Tasks
Create 3 tasks:
- vTaskR : Blink red led using core 0.
- vTaskG : Blink green led using core 1.
- vTaskB : Blink blue led with either core 0 or core 1 (tskNO_AFFINITY).

## FreeRTOS_Queue
Create a queue to communicate vTask1 with vTask2
