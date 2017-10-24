#include <MapleFreeRTOS821.h>
#define BOARD_LED_PIN PC13

int sensorValue; 
xSemaphoreHandle mutex1 = 0; 


static void vLEDFlashTask(void *pvParameters) {
    for (;;) {
        vTaskDelay(1000);
        digitalWrite(BOARD_LED_PIN, HIGH);
        vTaskDelay(50);
        digitalWrite(BOARD_LED_PIN, LOW);
    }
}

static void vSendBlue(void *pvParameters) {
    for (;;) {
        vTaskDelay(100);
        xSemaphoreTake(mutex1,100);
        Serial1.println(sensorValue);
        xSemaphoreGive(mutex1);  
        vTaskDelay(500);
    }
}

static void vSensorInput(void *pvParameters) {
    for (;;) {
     xSemaphoreTake(mutex1, 100);
     sensorValue = analogRead(PA0);
     xSemaphoreGive(mutex1);
     vTaskDelay(100); 
    }
}

void setup() {
    // initialize the digital pin as an output:
    pinMode(BOARD_LED_PIN, OUTPUT);
    Serial1.begin(9600); 
    mutex1 = xSemaphoreCreateMutex(); 
    
    xTaskCreate(vLEDFlashTask,
                "Task1",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
    xTaskCreate(vSendBlue,
                "Task2",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
    xTaskCreate(vSensorInput,
                "Task3",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
                
    vTaskStartScheduler();
}

void loop() {
    // Insert background code here
}


