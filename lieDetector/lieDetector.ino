#include <MapleFreeRTOS821.h>
#define BOARD_LED_PIN PA0
#define GREEN_BUTTON_PIN PB10
#define RED_BUTTON_PIN PB11
#define BUZZER_PIN PA1
#define SENSOR_PIN PA7

int sensorValue; 
bool programStart = false;
bool buttons;
int sensorMax = 0;


xSemaphoreHandle mutex1 = 0; 
xSemaphoreHandle mutex2 = 0; 
TaskHandle_t xHandle; 
//QueueHandle_t xSensorQueue; 


static void vLEDFlashTask(void *pvParameters) {
    for (;;) {
        vTaskDelay(1000);
        digitalWrite(BOARD_LED_PIN, HIGH);
        vTaskDelay(1000);
        digitalWrite(BOARD_LED_PIN, LOW);
    }
}

static void vSendBlue(void *pvParameters) {
    for (;;) {
        vTaskDelay(100);
        xSemaphoreTake(mutex1,100);
        //int sensorValue; 
        //xQueueReceive(xSensorQueue,&sensorValue, 100); 
        Serial.println(sensorValue);
        Serial.print("MAX: ");
        Serial.println(sensorMax);
        xSemaphoreGive(mutex1);  
        vTaskDelay(100);
    }
}

static void vSensorInput(void *pvParameters) {
    for (;;) {
     xSemaphoreTake(mutex1, 100);
     sensorValue = analogRead(SENSOR_PIN);
     xSemaphoreGive(mutex1);
     //xQueueSend(xSensorQueue, &sensorValue, 100); 
     if(sensorValue > sensorMax){
       digitalWrite(BUZZER_PIN, HIGH);
       vTaskDelay(500); 
       digitalWrite(BUZZER_PIN, LOW);  
     } 
     vTaskDelay(100); 
    }
}
/*
static void vBuzzerOut(void *pvParameters) {
    for (;;) {
     xSemaphoreTake(mutex1, 100);
     if(sensorValue > sensorMax){
       digitalWrite(BUZZER_PIN, HIGH);
       vTaskDelay(500); 
       digitalWrite(BUZZER_PIN, LOW);  
     }
     xSemaphoreGive(mutex1);
     vTaskDelay(100); 
   }
}
*/
static void vStartButton(void *pvParameters) {
    for (;;) {
      int buttonState = digitalRead(GREEN_BUTTON_PIN); 
      if(buttonState == HIGH && programStart == false){
        vTaskSuspendAll();
        Serial.println("------PROGRAM START-----");  
        Serial.println("------PROGRAM START-----");  
        xTaskResumeAll(); 
        programStart = true; 
      }
     vTaskDelay(100); 
    }
}

static void vCalibrateEDA(void* pvParameters) {
  for(int i = 0; i < 200; i++){
      int a = analogRead(SENSOR_PIN);
      if(a > sensorMax){
        sensorMax = a; 
      }      
    }
    sensorMax += 20;
}

static void vCheckButtons(void* pvParameters) {
  for(;;) {
    if(digitalRead(GREEN_BUTTON_PIN)) {
      buttons = true;      
    }    
    if(digitalRead(RED_BUTTON_PIN)) { 
      buttons = false;
      xSemaphoreGive(mutex2);
    }
  }
  
}

static void vLieDetector(void* pvParameters) {
  struct states *buttons = (struct states*) pvParameters;
  for(;;) {
    switch(buttons) {
      case true:
          xSemaphoreTake(mutex2, 1);
      break;
      case false: 
          
      break;
    }
  }
}



void setup() {
    // initialize the digital pin as an output:
    pinMode(BOARD_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
     
    Serial.begin(9600); 
    
    
    
    mutex1 = xSemaphoreCreateMutex(); 
    mutex2 = xSemaphoreCreateMutex();
    //xSensorQueue = xQueueCreate(10, sizeof(int)); 
    
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
                &xHandle);
    /*xTaskCreate(vBuzzerOut,
              "Task3",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 2,
              NULL);
            */ 
     xTaskCreate(vStartButton,
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


