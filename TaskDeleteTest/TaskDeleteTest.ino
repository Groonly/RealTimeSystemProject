//#include <wirish/wirish.h>
//#include "libraries/FreeRTOS/MapleFreeRTOS.h"
#include <MapleFreeRTOS821.h>

 
#define GREEN_BUTTON_PIN PB10
#define RED_BUTTON_PIN PB11
#define SENSOR_PIN PA7

xSemaphoreHandle mutex1 = 0; 
xTaskHandle vTaskEDA_Handle;

static void vTaskEDA(void* pvParameters) {
    int EDA
    while(digitalRead(RED_BUTTON_PIN) != HIGH) {
      analogRead(SENSOR_PIN);
      Serial.println);
    }
    vTaskDelete(vTaskEDA_Handle);    
}

static void vLieDetector(void *pvParameters){
  for(;;) {
   if(digitalRead(GREEN_BUTTON_PIN) == HIGH) {
      xSemaphoreTake(, tid);
      xTaskCreate(vTaskEDA,
                "Task0",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 10,
               &vTaskEDA_Handle);     
   }  
  }
}




void setup() {
    // initialize the digital pin as an output:  
   Serial.begin(9600);
   delay(2000);

   mutex1 =  
    
    xTaskCreate(vLieDetector,
                "Task0",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 10,
                NULL);    
                                
    vTaskStartScheduler();
}

void loop() {
    // Insert background code here
}


