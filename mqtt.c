//gcc mqtt.c -o mqtt.run -lpaho-mqtt3c -lwiringPi -lwiringPiDev -lm -lrt -lcrypt
//BIB FOR DISPLAY
//#include "display.h"

#include <lcd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

//BIB FOR MQTT
#include <MQTTClient.h>

//BIB FOR INTERRUPT, GPIO
#include <wiringPi.h>

#define MQTT_ADDRESS   "tcp://10.0.0.101:1883"
#define USER           "aluno"
#define PASSWORD       "@luno*123"
#define CLIENTID       "MQTTClientRASP"
#define QOS            2
#define TIMEOUT        1000L

#define NODEMCU_PUBLISH  "NODEMCU_PUBLISH"
#define NODEMCU_RECEIVE  "NODEMCU_RECEIVE"
#define SENSORS_HISTORY  "SENSORS_HISTORY"

// Pinos do Display LCD
#define RS  13
#define E   18
#define D4  21
#define D5  24
#define D6  26
#define D7  27

// Pinos botoes
#define BTN1 19
#define BTN2 23
#define BTN3 25

MQTTClient client;

int display_lcd;

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

// ONE FOR EACH SENSOR
Historic array_registros[10];

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void reconnect(void *context, char *cause);
void evaluateRecData(char *topicName, char *payload);
char *substring(char *src, int start, int end);
void updateHistory(char * sensor, int newValue);
char *createJson(int index);
void initArrayRegistros();
void initDisplay();
void write_textLCD(char *linha1, char *linha2);
void handle (void);



int main(int argc, char *argv[]){
   int rc;
   MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
   MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   MQTTClient_setCallbacks(client, &conn_opts, reconnect, on_message, NULL);

   conn_opts.username = USER;
   conn_opts.password = PASSWORD;

   rc = MQTTClient_connect(client, &conn_opts);

   if (rc != MQTTCLIENT_SUCCESS){
       printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
       exit(-1);
   }

    MQTTClient_subscribe(client, NODEMCU_RECEIVE, 0);            //Subscribing to MQTT topic that NodeMcu publishes
    
    wiringPiSetup();
    initDisplay();              // INIT LCD DISPLAY
    initArrayRegistros();      // INIT HISTORIC

	/*
	pinMode(BTN1, INPUT);                          // Set pin to output in case it's not
	pinMode(BTN2, INPUT);                         // Set pin to output in case it's not
	pinMode(BTN3, INPUT);                         // Set pin to output in case it's not
    
    wiringPiISR(BTN1, INT_EDGE_BOTH, &handle);
    wiringPiISR(BTN2, INT_EDGE_BOTH, &handle);
    wiringPiISR(BTN3, INT_EDGE_BOTH, &handle);
    
    stateBtn1 = digitalRead(BTN1);                     // Get initial state of pin
    stateBtn2 = digitalRead(BTN2);                     // Get initial state of pin
    stateBtn3 = digitalRead(BTN3); 
    
	piThreadCreate(BUTTON_INTERRUPTION);            // CREATING THREAD TO VERIFY INTERRUPTIONS 
    */

    system("cls || clear");
    write_textLCD("PBL - SD", "IoT");
    
    printf("PBL 3 - IoT: A Internet das Coisas. \n \n");
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
               write_textLCD("Solicitacao: ", "Status NodeMCU");
               sleep(3);

               publish(client, NODEMCU_PUBLISH, "30");
            }

            if(i == 1){                   ///this requests analog input value
               //system("cls || clear");
               printf("Ação: Solicita o valor da entrada analógica. \n");
               write_textLCD("Solicitacao: ", "Sen. Analogico");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "40");
            }

            if(i == 2){                   //this requests some digital input value
               //system("cls || clear");
               printf("Ação: Solicita o valor de uma das entradas digitais. \n");
               sleep(3);

               //LOOP TO COMMUTE DIGITAL SENSORS REQUESTS
               for (int idxSensor = 50; idxSensor < 59; idxSensor++){
                   sleep(2);
                   printf("Entrada digital: %d. \n \n", idxSensor);

                   char numStr[6] = "";
                   sprintf(numStr, "%d", idxSensor);               //CONVERT INTEGER TO STRING
                   
                   char text[15];
                   strcpy(text, "Sen. Digital ");
                   strcat(text, numStr);
                   write_textLCD("Solicitacao: ", text);
                   publish(client, NODEMCU_PUBLISH, numStr);
               }
            }

            if(i == 3){                   //this turn on the led
               //system("cls || clear");
               printf("Ação: Acendimento do LED da NodeMCU. \n");
               write_textLCD("Solicitacao: ", "Ligar LED");
               sleep(3);

               publish(client, NODEMCU_PUBLISH, "60");
            }

            if(i == 4){                   //this turn off the led
               i = -1;             // RESET THE LOOP

               //system("cls || clear");
               printf("Ação: Desligamento do LED do NodeMCU. \n");
               write_textLCD("Solicitacao: ", "Desligar LED");
               sleep(3);

               publish(client, NODEMCU_PUBLISH, "70");
               //system("cls || clear");
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

     //printf("Mensagem enviada! \n\rTopico: %s Mensagem: %s \n \n", topic, payload);
}

// METHOD CALLED WHENEVER A MESSAGE IS RECEIVED IN ANY SUBSCRIBED TOPIC
// Return: 1
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    //printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s \n \n", topicName, payload);
    evaluateRecData(topicName, payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void reconnect(void *context, char *cause){
     printf("CAIU AAAAAA");
     /*int rc = MQTTClient_connect(client, context);

    if (rc != MQTTCLIENT_SUCCESS){
        printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
        exit(-1);
    } 
    else { printf("Reconectado\n"); }

    MQTTClient_subscribe(client, NODEMCU_RECEIVE, 0);     */
}

// METHOD TO VERIFY WHAT WAS RECEIVED FROM WHICH TOPIC
// PARAM: TOPICNAME (topic), PAYLOAD (content received)
// Return: NEW STRING 
void evaluateRecData(char * topicName, char *payload){
    if(strcmp(topicName, "NODEMCU_RECEIVE") == 0){
        // 1F
        if (payload[0] == '1' && payload[1] == 'F'){
           write_textLCD("Resposta: ", "NodeMCU not ok");
        }

        // 00
        else if (payload[0] == '0' && payload[1] == '0'){
           write_textLCD("Resposta: ", "NodeMCU Ok");
        }

        // 01
        else if (payload[0] == '0' && payload[1] == '1'){
           char *analogInputValue = substring(payload, 2, sizeof(payload)+1);     // COPYING ONLY THE INPUT VALUE RECEIVED

           int value = atoi(analogInputValue);            // CONVERT STRING TO INT
           updateHistory("A0", value);
           write_textLCD("Sen. Analogico: ", analogInputValue);
        }

        //02
        else if (payload[0] == '0' && payload[1] == '2'){
           // COPYING THE DIGITAL SENSOR & INPUT VALUE RECEIVED       
           char *digitalSensor = substring(payload, 2, 4);
           char *digitalInputValue = substring(payload, 4, sizeof(payload)*sizeof(payload));

           int value = atoi(digitalInputValue);         // CONVERT STRING TO INT
           //printf("Sensor: %s. \n", digitalSensor);
           //printf("Valor Sensor: %s , %d. \n", digitalInputValue, value);
           char text[15];
           strcpy(text, "Sen. Digital ");
           strcat(text, digitalSensor);
           
           write_textLCD(text, digitalInputValue);
           updateHistory(digitalSensor, value);
        }

        else{
            write_textLCD("Resposta: ", "NodeMCU not ok");
        }
    }
    
    return;
}

// METHOD TO CUT AND GET STRING FROM START TO END
// PARAM: SRC (string to be splited), START (first index), END (last index)
// Return: NEW STRING 
char *substring(char *src, int start, int end){
	int len = end - start;                                    // get the length of the destination string
	char *dest = (char*)malloc(sizeof(char) * (len + 1));    // allocate (len + 1) chars for destination (+1 for extra null character)
	strncpy(dest, (src + start), len);                       // start with start'th char and copy `len` chars into the destination
	return dest;
}

void updateHistory(char *sensor, int newValue){     
     int idx = 0;
     struct timeval now;                        // timestamp
     gettimeofday(&now, NULL);
     
     char *json = (char*)malloc( (sizeof(char) * (200)) + 1 );
     char sensorTopic[20];
     strcpy(sensorTopic, "SENSORS_HISTORY/");
     strcat(sensorTopic, sensor);
     
     if(strcmp(sensor, "A0") == 0){                       
        idx = ++array_registros[0].last_modified;
        if(array_registros[0].last_modified >= 9){ array_registros[0].last_modified = -1; }
        
        array_registros[0].historic[idx] = newValue;
        array_registros[0].timestamps[idx] = now.tv_usec;
        strcpy(json, createJson(0));
        publish(client, sensorTopic, json);
     }
     
     else if(strcmp(sensor, "D0") == 0){
        idx = ++array_registros[1].last_modified;
        if(array_registros[1].last_modified >= 9){ array_registros[1].last_modified = -1; }
        
        array_registros[1].historic[idx] = newValue;
        array_registros[1].timestamps[idx] = now.tv_usec;
        strcpy(json, createJson(1));
        publish(client, sensorTopic, json);
     }
     
     else if(strcmp(sensor, "D1") == 0){
        idx = ++array_registros[2].last_modified;
        if(array_registros[2].last_modified >= 9){ array_registros[2].last_modified = -1; }
        
        array_registros[2].historic[idx] = newValue;
        array_registros[2].timestamps[idx] = now.tv_usec;
        strcpy(json, createJson(2));
        publish(client, sensorTopic, json);
     }
     
     else if(strcmp(sensor, "D2") == 0){
       idx = ++array_registros[3].last_modified;
       if(array_registros[3].last_modified >= 9){ array_registros[3].last_modified = -1; }
        
       array_registros[3].historic[idx] = newValue;
       array_registros[3].timestamps[idx] = now.tv_usec;
       strcpy(json, createJson(3));
       publish(client, sensorTopic, json);
     }
     
     else if(strcmp(sensor, "D3") == 0){
       idx = ++array_registros[4].last_modified;
       if(array_registros[4].last_modified >= 9){ array_registros[4].last_modified = -1; }
        
       array_registros[4].historic[idx] = newValue;
       array_registros[4].timestamps[idx] = now.tv_usec;
       strcpy(json, createJson(4));
       publish(client, sensorTopic, json);
     }
     
     else if(strcmp(sensor, "D4") == 0){
       idx = ++array_registros[5].last_modified;
       if(array_registros[5].last_modified >= 9){ array_registros[5].last_modified = -1; }
        
       array_registros[5].historic[idx] = newValue;
       array_registros[5].timestamps[idx] = now.tv_usec;
       strcpy(json, createJson(5));
       publish(client, sensorTopic, createJson(5));
     }
     
     else if(strcmp(sensor, "D5") == 0){
       idx = ++array_registros[6].last_modified;
       if(array_registros[6].last_modified >= 9){ array_registros[6].last_modified = -1; }
        
       array_registros[6].historic[idx] = newValue;
       array_registros[6].timestamps[idx] = now.tv_usec;
       strcpy(json, createJson(6));
       publish(client, sensorTopic, json);
     }
     
     else if(strcmp(sensor, "D6") == 0){
       idx = ++array_registros[7].last_modified;
       if(array_registros[7].last_modified >= 9){ array_registros[7].last_modified = -1; }
        
       array_registros[7].historic[idx] = newValue;
       array_registros[7].timestamps[idx] = now.tv_usec;
       strcpy(json, createJson(7));
       publish(client, sensorTopic, json);
     }
     
     else if(strcmp(sensor, "D7") == 0){
       idx = ++array_registros[8].last_modified;
       if(array_registros[8].last_modified >= 9){ array_registros[8].last_modified = -1; }
        
       array_registros[8].historic[idx] = newValue;
       array_registros[8].timestamps[idx] = now.tv_usec;
       strcpy(json, createJson(8));
       publish(client, sensorTopic, json);
     }
     
     else if(strcmp(sensor, "D8") == 0){
       idx = ++array_registros[9].last_modified;
       if(array_registros[9].last_modified >= 9){ array_registros[9].last_modified = -1; }
        
       array_registros[9].historic[idx] = newValue;
       array_registros[9].timestamps[idx] = now.tv_usec;
       strcpy(json, createJson(9));
       publish(client, sensorTopic, json);
     }
     
     else{
       printf("Sensor %s desconhecido!!! \n \n", sensor);
     }
     
     /*for(int z = 0; z < 10; z++){
       printf("Array Registros: %d , %d , %d \n", array_registros[0].last_modified, array_registros[0].historic[z], array_registros[0].timestamps[z]);
     } */
     printf("JSON: %s \n\n", json);
     return;
}


char *createJson(int index){
     char *json = (char*)malloc((sizeof(char) * (200)) + 1);
     strcpy(json, "{\"historico\": ");           //Ex.: { "historico\":

     // LOOP TO CONCAT THE START OF JSON
     for(int i = 0; i < 10; i++){
        char valueHistoric[20];
        sprintf(valueHistoric, "%d", array_registros[index].historic[i]);               //CONVERT INTEGER TO STRING

        if(i == 0){
           strcat(json, "\"[");                              //Ex.: { "\sensor\":\"A0\", \"historico\":\"[
           strcat(json, valueHistoric); //Ex.: { "\sensor\":\"A0\", \"historico\":\"[0
        }

        else if(i == 9){
           strcat(json, ", ");
           strcat(json, valueHistoric);
           strcat(json, "]");
        }

        else{
           strcat(json, ", ");
           strcat(json, valueHistoric);
        }
     }
     
     strcat(json, "\", \"timestamps\": ");           //Ex.: { "\sensor\":\"A0\", \"historico\":
     
     // LOOP TO CONCAT THE TIMESTAMPS TO JSON
     for(int j = 0; j < 10; j++){
        char *valueTimestamp = (char*)malloc((sizeof(char) * (50)) + 1);
        sprintf(valueTimestamp, "%d", array_registros[index].timestamps[j]);               //CONVERT INTEGER TO STRING        

        if(j == 0){
           strcat(json, "\"[");                              //Ex.: { "\sensor\":\"A0\", \"historico\":\"[
           strcat(json, valueTimestamp);                     //Ex.: { "\sensor\":\"A0\", \"historico\":\"[0
        }

        else if(j == 9){
           strcat(json, ", ");
           strcat(json, valueTimestamp);
           strcat(json, "]\"}");
        }

        else{
           strcat(json, ", ");
           strcat(json, valueTimestamp);
        }
     }
     return json;
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

/**
 * Realiza as rotinas de inicializacao do display
 */
void initDisplay(){
    display_lcd = lcdInit(2, 16, 4, RS, E, D4, D5, D6, D7, 0, 0, 0, 0) ;
    lcdHome(display_lcd);
    lcdClear(display_lcd);
}  

/**
 * Escreve em duas linhas do display LCD
 * @param linha1 - Primeira linha do display
 * @param linha2 - Segunda linha do display
*/
void write_textLCD(char *linha1, char *linha2) {
    lcdHome(display_lcd);
    lcdClear(display_lcd);

    // escreve na primeira linha
    lcdPosition(display_lcd, 0, 0);
    lcdPuts(display_lcd, linha1);

    // escreve na segunda linha
    lcdPosition(display_lcd, 0, 1);
    lcdPuts(display_lcd, linha2);
}
