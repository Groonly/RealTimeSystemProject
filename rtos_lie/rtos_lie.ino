//LED PB3 - PB9(NOT PB4) (1, 2, 3, 5, 6, 7) (4 = PB15) PB12- PB14 (8 - 10)

#include <MapleFreeRTOS821.h>
#define LED_PIN PC13
#define GREEN_BUTTON_PIN PB10
#define RED_BUTTON_PIN PB11
#define EDA_SENSOR_PIN PA0
#define PULSE_SENSOR_PIN PA1

int ledArray[]= {PB3, PB15, PB5, PB6, PB7, PB8, PA9, PB12, PB13, PB14}; 
int EDASensorValue; 
int sensorMax = 0;
int heartHigh, heartLow,loopCount, mapped;
int pulseData[200];


xSemaphoreHandle mutex1 = 0; 
//static TaskHandle_t xHandle1 = NULL, xHandle2 = NULL; 


/*Flash onboard LED 1 sec ON 1 sec OFF*/
static void vLEDFlashTask(void *pvParameters) {
    for (;;) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
        digitalWrite(LED_PIN, HIGH);
        Serial.println("---LEDTASK---");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        digitalWrite(LED_PIN, LOW);
    }
}

/*Handeling sending over uart ----Can maybe be placed inside EDA task, every sesnorTask handel its own sending?*/
static void vUartTX(void *pvParameters) {
    for (;;) {
        xSemaphoreTake(mutex1,100);
        Serial.println(EDASensorValue);
        Serial.print("---MAX: ---");
        Serial.println(sensorMax);
        xSemaphoreGive(mutex1);  
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}
/*Reading input from A0 (EDA sensor) every 10 mS*/
static void vEDAInput(void *pvParameters) {
    for (;;) {
     xSemaphoreTake(mutex1, 100);
     EDASensorValue = analogRead(EDA_SENSOR_PIN);
     Serial.println("---EDA--SENSOR TASK---");
     xSemaphoreGive(mutex1);
     vTaskDelay(10/portTICK_PERIOD_MS); 
    }
}
/*Reading input from A1 (Pulse sensor) Top priority task needs to be handled every 5 ms to create a good curve*/
static void vPulseInput(void *pvParameters) {
    for (;;) {
      vTaskSuspendAll();
     //Reset high and low values 
     heartHigh = 0;
     heartLow = 4096; 

     for(loopCount = 200; loopCount > 0; loopCount--){
        //Move everything in the array one step back to make room for new value
        pulseData[loopCount] = pulseData[loopCount-1];
               
        //Check if  there is some old value that higher then the current one 
        if(pulseData[loopCount] > heartHigh){ //if its higher assigne that value
          heartHigh = pulseData[loopCount]; 
        }
        //Same but with the lower
        if(heartLow < pulseData[loopCount]){
          heartLow = pulseData[loopCount]; 
        }
     }
     pulseData[0] = analogRead(PULSE_SENSOR_PIN); 
     
     Serial.println("---PULSE-SENSOR-TASK---");
     Serial.println(pulseData[0]);
     for(int i = 0; i < 10; i++){
        //Needs to be changed when depending on input spaan.
        if(pulseData[0] > (40* i * 3 )) digitalWrite(ledArray[i], HIGH);
        else digitalWrite(ledArray[i], LOW); 
      }
     xTaskResumeAll();
     vTaskDelay(5/portTICK_PERIOD_MS);
    }
}
/*Reading input from button press, two buttons implemnted, calibrates MAX value EDA*/
static void vCheckButtons(void* pvParameters) {
  for(;;) {
    if(digitalRead(GREEN_BUTTON_PIN)){ 
      //xTaskNotifyGive(xHandle1); 
      for(int i = 0; i < 10; i++){              
        Serial.println("--------------------------CALIBRATION TASK--------------------------------------");
        int a = analogRead(EDA_SENSOR_PIN);
        if(a > sensorMax){
        sensorMax = a; 
      }    
    } 
    if(digitalRead(RED_BUTTON_PIN)){
      
    }
    Serial.println("---BUTTON TASK----");  
    //Good delay for debouncing
    vTaskDelay(75/portTICK_PERIOD_MS);  
    } 
  }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
    pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
    
    for(int i = 0; i < 10; i++){
      pinMode(ledArray[i], OUTPUT); 
    }
    
    for(int i = 0; i < 10; i++){
      digitalWrite(ledArray[i], LOW); 
      delay(100); 
    }
       
    Serial.begin(9600); 
    
    mutex1 = xSemaphoreCreateMutex(); 
    
    xTaskCreate(vLEDFlashTask,
                "Task1",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
    xTaskCreate(vUartTX,
                "Task2",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
    xTaskCreate(vEDAInput,
                "Task3",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
     xTaskCreate(vCheckButtons,
              "Task4",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 2,
              NULL);  
    xTaskCreate(vPulseInput,
              "Task5",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 3,
              NULL);
                        
    vTaskStartScheduler();
}


void loop() {
    // Insert background code here
}


