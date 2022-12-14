//gcc mqtt.c -o mqtt.run -lpaho-mqtt3c -lwiringPi -pthread -lm -lrt -lcrypt
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

//BIB FOR MQTT
#include <MQTTClient.h>

//BIB FOR THREAD, INTERRUPT, GPIO
#include <wiringPi.h>
#include <pthread.h>

//BIB FOR DISPLAY
//#include "display.h"

#define MQTT_ADDRESS   "tcp://10.0.0.101:1883"
#define USER           "aluno"
#define PASSWORD       "@luno*123"
#define CLIENTID       "MQTTClientRASP"
#define QOS            2
#define TIMEOUT        4000L

#define NODEMCU_PUBLISH  "NODEMCU_PUBLISH"
#define NODEMCU_RECEIVE  "NODEMCU_RECEIVE"
#define SENSORS_HISTORY  "SENSORS_HISTORY"

#define BTN1 19
#define BTN2 23
#define BTN3 25

MQTTClient client;

// Current state of the buttons 1, 2, 3
static volatile int stateBtn1;
static volatile int stateBtn2;
static volatile int stateBtn3;

typedef struct Historic {
  char *sensor;
  int historic[10];
  int timestamps[10];
  int last_modified;
} Historic;

Historic array_registros[10];

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void evaluateRecData(char *topicName, char *payload);
char *substring(char *src, int start, int end);
void updateHistory(char * sensor, int newValue);
void initArrayRegistros();
void handle (void);

/*
PI_THREAD (BUTTON_INTERRUPTION){
  for (;;){
     stateBtn1 = digitalRead(BTN1);                     // Get initial state of pin
     stateBtn2 = digitalRead(BTN2);                     // Get initial state of pin
     stateBtn3 = digitalRead(BTN3);
    
     printf("VALUE 1: %d \n \n", stateBtn1);
     printf("VALUE 2: %d \n \n", stateBtn2);
     printf("VALUE 3: %d \n \n", stateBtn3);
     
     if (stateBtn1 == 0) {
        printf("BUTTON 1 PRESSED \n \n");
        publish(client, NODEMCU_PUBLISH, "30");         //this checks the NodeMCU status.
     }
    
     if (stateBtn2 == 0) {
        printf("BUTTON 2 PRESSED \n \n");
    	publish(client, NODEMCU_PUBLISH, "40");         ///this requests analog input value
     }
    
     if (stateBtn3 == 0) {
        printf("BUTTON 3 PRESSED \n \n");
    	publish(client, NODEMCU_PUBLISH, "60");         // this turn on the led,
    	sleep(2);                                       // wait 2 seconds
    	publish(client, NODEMCU_PUBLISH, "70");         // then turns off the led
     }
  }
  
} */

int main(int argc, char *argv[]){
   int rc;
   MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
   MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

   conn_opts.username = USER;
   conn_opts.password = PASSWORD;

   rc = MQTTClient_connect(client, &conn_opts);

   if (rc != MQTTCLIENT_SUCCESS){
       printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
       exit(-1);
   }

    MQTTClient_subscribe(client, NODEMCU_RECEIVE, 0);            //Subscribing to MQTT topic that NodeMcu publishes

    initArrayRegistros();                         // INIT HISTORIC

	/*wiringPiSetup();                                // Init  -- use the physical pin number on RPi P1 connector	
	pinMode(BTN1, INPUT);                          // Set pin to output in case it's not
	pinMode(BTN2, INPUT);                         // Set pin to output in case it's not
	pinMode(BTN3, INPUT);                         // Set pin to output in case it's not
    
    wiringPiISR(BTN1, INT_EDGE_BOTH, &handle);
    wiringPiISR(BTN2, INT_EDGE_BOTH, &handle);
    wiringPiISR(BTN3, INT_EDGE_BOTH, &handle);
    
    stateBtn1 = digitalRead(BTN1);                     // Get initial state of pin
    stateBtn2 = digitalRead(BTN2);                     // Get initial state of pin
    stateBtn3 = digitalRead(BTN3); 
    
	piThreadCreate(BUTTON_INTERRUPTION);            // CREATING THREAD TO VERIFY INTERRUPTIONS */

    //system("cls || clear");
    printf("PBL - Interfaces de E/S. \n \n");
    printf("As ações seguirão a seguinte ordem: \n");
    printf("1 - Solicita a situação atual do NodeMCU. \n");
    printf("2 - Solicita o valor da entrada analógica. \n");
    printf("3 - Solicita o valor de uma das entradas digitais. \n");
    printf("4 - Acendimento do led da NodeMCU. \n");
    printf("5 - Desligamento do led da NodeMCU. \n \n");
    sleep(2);

    for(int i = 0; i <= 4; i++){           // LOOP TO ENGAGE AUTOMATIC ACTIONS          
            if(i == 0){                   //this checks the NodeMCU status.
               printf("Ação: Solicita a situação atual do NodeMCU. \n");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "30");
            }

            if(i == 1){                   ///this requests analog input value
               //system("cls || clear");
               printf("Ação: Solicita o valor da entrada analógica. \n");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "40");
            }

            if(i == 2){                   //this requests some digital input value
               //system("cls || clear");
               printf("Ação: Solicita o valor de uma das entradas digitais. \n");
               sleep(1);

               //LOOP TO COMMUTE DIGITAL SENSORS REQUESTS
               for (int idxSensor = 50; idxSensor < 59; idxSensor++){
                   sleep(2);
                   printf("Entrada digital: %d. \n", idxSensor);

                   char numStr[6] = "";
                   sprintf(numStr, "%d", idxSensor);               //CONVERT INTEGER TO STRING

                   publish(client, NODEMCU_PUBLISH, numStr);
               }
            }

            if(i == 3){                   //this turn on the led
               //system("cls || clear");
               printf("Ação: Acendimento do LED da NodeMCU. \n");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "60");
            }

            if(i == 4){                   //this turn off the led
               i = -1;             // RESET THE LOOP

               //system("cls || clear");
               printf("Ação: Desligamento do LED do NodeMCU. \n");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "70");
            }
    }

    return rc;
}

// METHOD THAT WRITES IN THE TOPIC SOME MESSAGE
// Param: CLIENT (instance of MQTTClient), TOPIC (where to write), PAYLOAD (message to be written)
// Return: void
void publish(MQTTClient client, char* topic, char* payload) {
     //sleep(3);
     MQTTClient_message pubmsg = MQTTClient_message_initializer;

     pubmsg.payload = payload;
     pubmsg.payloadlen = strlen(pubmsg.payload);
     pubmsg.qos = QOS;
     pubmsg.retained = 0;
     MQTTClient_deliveryToken token;
     MQTTClient_publishMessage(client, topic, &pubmsg, &token);
     MQTTClient_waitForCompletion(client, token, TIMEOUT);

     printf("Mensagem enviada! \n\rTopico: %s Mensagem: %s \n \n", topic, payload);
}

// METHOD CALLED WHENEVER A MESSAGE IS RECEIVED IN ANY SUBSCRIBED TOPIC
// Return: 1
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s \n \n", topicName, payload);
    evaluateRecData(topicName, payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// METHOD TO VERIFY WHAT WAS RECEIVED FROM WHICH TOPIC
// PARAM: TOPICNAME (topic), PAYLOAD (content received)
// Return: NEW STRING 
void evaluateRecData(char * topicName, char *payload){
    if(strcmp(topicName, "NODEMCU_RECEIVE") == 0){
        // 1F
        if (payload[0] == '1' && payload[1] == 'F'){
           //write_textLCD("NodeMcu Not Ok");
           printf("Resposta 1F: %s. \n", payload);
        }

        // 00
        else if (payload[0] == '0' && payload[1] == '0'){
           //write_textLCD("NodeMcu Ok");
           printf("Resposta 00: %s. \n", payload);
        }

        // 01
        else if (payload[0] == '0' && payload[1] == '1'){
           char *analogInputValue = substring(payload, 2, sizeof(payload)+1);     // COPYING ONLY THE INPUT VALUE RECEIVED

           int value = atoi(analogInputValue);            // CONVERT STRING TO INT
           updateHistory("A0", value);
           printf("Resposta 01: %s. \n", analogInputValue);
        }

        //02
        else if (payload[0] == '0' && payload[1] == '2'){
           // COPYING THE DIGITAL SENSOR & INPUT VALUE RECEIVED       
           char *digitalSensor = substring(payload, 2, 4);
           char *digitalInputValue = substring(payload, 4, sizeof(payload)*sizeof(payload));

           int value = atoi(digitalInputValue);         // CONVERT STRING TO INT
           printf("Sensor: %s. \n", digitalSensor);
           printf("Valor Sensor: %s , %d. \n", digitalInputValue, value);
           updateHistory(digitalSensor, value);
        }

        else{
            //write_textLCD("NodeMcu Not Ok");
            printf("Resposta desconhecida: %s. \n", payload);
        }
    }

    else if(strcmp(topicName, "NODEMCU_PUBLISH") == 0){
         printf("Resposta NODEMCU_PUBLISH: %s. \n", payload);
    }
}

// METHOD TO CUT AND GET STRING FROM START TO END
// PARAM: SRC (string to be splited), START (first index), END (last index)
// Return: NEW STRING 
char *substring( char *src, int start, int end){
	int len = end - start;                                    // get the length of the destination string
	char *dest = (char*)malloc(sizeof(char) * (len + 1));    // allocate (len + 1) chars for destination (+1 for extra null character)
	strncpy(dest, (src + start), len);                       // start with start'th char and copy `len` chars into the destination
	return dest;
}

void updateHistory(char *sensor, int newValue){
     printf("Sensor: %s \n \n", sensor);
     printf("New Value: %d \n \n", newValue);
     
     int idx = 0;
     int idx2 = 0;
     struct timeval now;                        // timestamp
     gettimeofday(&now, NULL);
     
     if(strcmp(sensor, "A0") == 0){                       
        idx = ++array_registros[0].last_modified;
        idx2 = 0;
        if(idx >= 9){ array_registros[0].last_modified = -1; }
        
        array_registros[0].historic[idx] = newValue;
        array_registros[0].timestamps[idx] = now.tv_usec;
        //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D0") == 0){
        idx = ++array_registros[1].last_modified;
        idx2 = 1;
        if(idx >= 9){ array_registros[1].last_modified = -1; }
        
        array_registros[1].historic[idx] = newValue;
        array_registros[1].timestamps[idx] = now.tv_usec;
        //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D1") == 0){
        idx = ++array_registros[2].last_modified;
        idx2 = 2;
        if(idx >= 9){ array_registros[2].last_modified = -1; }
        
        array_registros[2].historic[idx] = newValue;
        array_registros[2].timestamps[idx] = now.tv_usec;
        //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D2") == 0){
       idx = ++array_registros[3].last_modified;
       idx2 = 3;
       if(idx >= 9){ array_registros[3].last_modified = -1; }
        
       array_registros[3].historic[idx] = newValue;
       array_registros[3].timestamps[idx] = now.tv_usec;
       //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D3") == 0){
       idx = ++array_registros[4].last_modified;
       idx2 = 4;
       if(idx >= 9){ array_registros[4].last_modified = -1; }
        
       array_registros[4].historic[idx] = newValue;
       array_registros[4].timestamps[idx] = now.tv_usec;
       //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D4") == 0){
       idx = ++array_registros[5].last_modified;
       idx2 = 5;
       if(idx >= 9){ array_registros[5].last_modified = -1; }
        
       array_registros[5].historic[idx] = newValue;
       array_registros[5].timestamps[idx] = now.tv_usec;
       //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D5") == 0){
       idx = ++array_registros[6].last_modified;
       idx2 = 6;
       if(idx >= 9){ array_registros[6].last_modified = -1; }
        
       array_registros[6].historic[idx] = newValue;
       array_registros[6].timestamps[idx] = now.tv_usec;
       //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D6") == 0){
       idx = ++array_registros[7].last_modified;
       idx2 = 7;
       if(idx >= 9){ array_registros[7].last_modified = -1; }
        
       array_registros[7].historic[idx] = newValue;
       array_registros[7].timestamps[idx] = now.tv_usec;
       //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D7") == 0){
       idx = ++array_registros[8].last_modified;
       idx2 = 8;
       if(idx >= 9){ array_registros[8].last_modified = -1; }
        
       array_registros[8].historic[idx] = newValue;
       array_registros[8].timestamps[idx] = now.tv_usec;
       //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else if(strcmp(sensor, "D8") == 0){
       idx = ++array_registros[9].last_modified;
       idx2 = 9;    
       if(idx >= 9){ array_registros[9].last_modified = -1; }
        
       array_registros[9].historic[idx] = newValue;
       array_registros[9].timestamps[idx] = now.tv_usec;
       //publish(client, SENSORS_HISTORY, "JSON_HERE");
     }
     
     else{
       printf("Sensor %s desconhecido!!! \n \n", sensor);
     }
     
     for(int z = 0; z < 10; z++){
       printf("Array Registros: [%d] , %d , %d , %d \n", idx2, array_registros[idx2].last_modified, array_registros[idx2].historic[z], array_registros[idx2].timestamps[z]);
     }
}

// METHOD TO INITIALIZE THE VARIABLE 'array_registros'
// Return: void
void initArrayRegistros(){              // [A0, D0, D1, D2, D3, D4, D5, D6, D7, D8]
     array_registros[0].sensor = "A0";
     array_registros[1].sensor = "D0";
     array_registros[2].sensor = "D1";
     array_registros[3].sensor = "D2";
     array_registros[4].sensor = "D3";
     array_registros[5].sensor = "D4";
     array_registros[6].sensor = "D5";
     array_registros[7].sensor = "D6";
     array_registros[8].sensor = "D7";
     array_registros[9].sensor = "D8";
     
     for(int i = 0; i < 10; i++){
        array_registros[i].last_modified = -1;
        
        for(int j = 0; j < 10; j++){
           array_registros[i].historic[j] = 0;
           array_registros[i].timestamps[j] = 0;
        }
     }
}


// METHOD TO HANDLE BUTTON INTERRUPTIONS
// PARAM: void
// Return: void
void handle(void){
     stateBtn1 = digitalRead(BTN1);                     // Get initial state of pin
     stateBtn2 = digitalRead(BTN2);                     // Get initial state of pin
     stateBtn3 = digitalRead(BTN3);

     /*printf("VALUE 1: %d \n \n", stateBtn1);
     printf("VALUE 2: %d \n \n", stateBtn2);
     printf("VALUE 3: %d \n \n", stateBtn3); */

     if (stateBtn1 == 0) {
        printf("BUTTON 1 PRESSED \n \n");
        publish(client, NODEMCU_PUBLISH, "30");         //this checks the NodeMCU status.
     }

     if (stateBtn2 == 0) {
        printf("BUTTON 2 PRESSED \n \n");
    	publish(client, NODEMCU_PUBLISH, "40");         ///this requests analog input value
     }

     if (stateBtn3 == 0) {
        printf("BUTTON 3 PRESSED \n \n");
    	publish(client, NODEMCU_PUBLISH, "60");         // this turn on the led,
    	sleep(2);                                       // wait 2 seconds
    	publish(client, NODEMCU_PUBLISH, "70");         // then turns off the led
     }
}
