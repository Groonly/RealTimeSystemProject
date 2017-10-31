#include <MapleFreeRTOS821.h>
//#include "utility/semphr.h"
#define GREEN_BUTTON PB6
#define RED_BUTTON PB7
#define EDA PA0

enum states {
  On, Off
};

enum states state;

static void OnOffTask(void* pvParameters);
static void EDAInitTask(void* pvParameters); // Varför ta medelvärde?
static void EDATask(void* pvParameters);
static void OximeterTask(void* pvParameters);
void vTaskCreateCheck(BaseType_t xTask);

TaskHandle_t OnOffTask_Handle;
TaskHandle_t EDAInitTask_Handle;
TaskHandle_t EDATask_Handle;
TaskHandle_t OximeterTask_Handle;

BaseType_t xReturn;

int EDAThreshold = 0;

void setup() {  
  Serial.begin(9600);
  delay(2000); // Vänta på att Serialen verkligen har startats  
  pinMode(GREEN_BUTTON, INPUT);
  pinMode(RED_BUTTON, INPUT);  

  state = Off;
  xReturn = xTaskCreate(OnOffTask, "OnOff", configMINIMAL_STACK_SIZE, NULL, 1, &OnOffTask_Handle);
  
  xReturn = xTaskCreate(EDAInitTask, "EDAInit", configMINIMAL_STACK_SIZE, NULL, 2, &EDAInitTask_Handle);  
  vTaskSuspend(EDAInitTask_Handle);
  
  xReturn = xTaskCreate(EDATask, "EDATask", configMINIMAL_STACK_SIZE, NULL, 3, &EDATask_Handle); 
  vTaskSuspend(EDATask_Handle);
  
  xReturn = xTaskCreate(OximeterTask, "OximeterTask", configMINIMAL_STACK_SIZE, NULL, 4, &OximeterTask_Handle);                       
  vTaskSuspend(OximeterTask_Handle);
  
  vTaskStartScheduler();    
}

void loop() {  
}

/* Tasks */
static void OnOffTask(void* pvParameters) 
{
  for(;;) {         
    if(digitalRead(GREEN_BUTTON) == HIGH && state == Off) {         
      state = On; 
      vTaskResume(EDAInitTask_Handle);  
      vTaskResume(OximeterTask_Handle);      
    } else if(state == On) {                
       EDAThreshold = 0;
       state = Off;
       vTaskDelay(1000);
    }       
    //Serial.println(".");    
  }      
}

static void EDAInitTask(void* pvParameters) {  
    for(;;) {      
      int sum = 0;
      for(int i = 0; i < 100; i++) {
          sum += digitalRead(EDA);
      }
      EDAThreshold = sum / 100;
      
      vTaskSuspend(EDAInitTask_Handle); // Suspenda sig själv, går tillbaks till OnOffTask    
    }    
}

static void EDATask(void* pvParameters) {       
  int EDAValRaw = 0;
  for(;;) {    
    if(digitalRead(RED_BUTTON) == HIGH) {
        state = Off;        
        //vTaskPrioritySet(OnOffTask_Handle, 20);                 
        vTaskResume(OnOffTask_Handle); // OnOffTask har lägst prio hoppar inte dit än. Bör igentligen hoppa till OximeterTask där den suspendats men OximeterTask kommer vara suspendad då.
        vTaskSuspend(OximeterTask_Handle); // Suspenda OximeterTask, kommer börja här ifrån när det unsuspendas      
        if(state == Off) {
           vTaskSuspend(EDATask_Handle); // Suspenda sig själv, kommer börja härifrån när den Unsuspendas.            
        }       
        vTaskSuspend(OnOffTask_Handle); // Suspenda så det inte kan hoppa till denna task
    } else {
      // Läs av EDA-proben
      EDAValRaw = analogRead(EDA);
      if(EDAValRaw > EDAThreshold) {
          // Buzzerljud eller hoppa till annan task.
      }
    }
    vTaskDelay(20/portTICK_PERIOD_MS); // 20 ticks delay
  }
  
}

static void OximeterTask(void* pvParameters) {   
  Serial.println("I OximeterTask.");
  vTaskResume(EDATask_Handle);
  vTaskSuspend(OnOffTask_Handle); 
      
  for(;;) {
    if(digitalRead(RED_BUTTON) == HIGH) {
      state = Off;   
      // vTaskPrioritySet(OnOffTask_Handle, 20);
      vTaskResume(OnOffTask_Handle); //OnOffTask har lägst prio, kommer inte hoppa dit försän resten av tasken har suspendats.
      vTaskSuspend(EDATask_Handle); // Suspenda EDATask, kommer börja här ifrån när det unsuspendas
      vTaskSuspend(OximeterTask_Handle); // Suspenda sig själv, kommer börja härifrån när den Unsuspendas.   
      vTaskSuspend(OnOffTask_Handle); // Suspenda så det inte kan hoppa till denna task  
    } else {
      Serial.println(analogRead(PA1)); 
      vTaskDelay(5/portTICK_PERIOD_MS);            
    }    
  }    
  
}


