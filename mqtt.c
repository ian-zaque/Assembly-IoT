//gcc mqtt.c -o mqtt.run -lpaho-mqtt3c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//BIB FOR MQTT
#include <MQTTClient.h>

//BIB FOR THREAD AND INTERRUPT
#include <wiringPi.h>

//BIB FOR DISPLAY
//#include "display.h"

#define MQTT_ADDRESS   "tcp://10.0.0.101:1883"
#define USER           "aluno"
#define PASSWORD       "@luno*123"
#define CLIENTID       "MQTTClientRASP"
#define QOS            1
#define TIMEOUT        10000L

#define NODEMCU_PUBLISH  "NODEMCU_PUBLISH"
#define NODEMCU_RECEIVE  "NODEMCU_RECEIVE"

MQTTClient client;

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void evaluateRecData(char *topicName, char *payload);

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

    MQTTClient_subscribe(client, NODEMCU_RECEIVE, 0);

    //PI_THREAD (myThread){

    //}

    system("cls || clear");
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
               printf("Ação: Solicita a situação atual do NodeMCU. \n \n");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "30");
            }

            if(i == 1){                   ///this requests analog input value
               //system("cls || clear");
               printf("Ação: Solicita o valor da entrada analógica. \n \n");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "40");
            }

            if(i == 2){                   //this requests some digital input value
               //system("cls || clear");
               printf("Ação: Solicita o valor de uma das entradas digitais. \n \n");
               sleep(1);
               
               for (int sensorIdx = 50; sensorIdx < 59; sensorIdx++){   //LOOP TO COMMUTE DIGITAL SENSORS
                   sleep(2);
                   printf("Entrada digital: %d. \n \n", sensorIdx);
                   
                   char numStr[6] = "";
                   sprintf(numStr, "%d", sensorIdx);               //CONVERT INTEGER TO STRING
                   
                   publish(client, NODEMCU_PUBLISH, numStr);
               }
            }

            if(i == 3){                   //this turn on the led
               //system("cls || clear");
               printf("Ação: Acendimento do LED da NodeMCU. \n \n");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "60");
            }

            if(i == 4){                   //this turn off the led
               i = -1;             // RESET THE LOOP

               //system("cls || clear");
               printf("Ação: Desligamento do LED do NodeMCU. \n \n");
               sleep(1);

               publish(client, NODEMCU_PUBLISH, "70");
            }

    }

   while(1){
       /*
        * este client opera por "interrupcao", ou seja, opera em funcao do que eh recebido no callback de recepcao de 
        * mensagens MQTT. Portanto, neste laco principal nao eh preciso fazer nada.
        */
   }
}

//SEND MESSAGE
void publish(MQTTClient client, char* topic, char* payload) {
     MQTTClient_message pubmsg = MQTTClient_message_initializer;

     pubmsg.payload = payload;
     pubmsg.payloadlen = strlen(pubmsg.payload);
     pubmsg.qos = 2;
     pubmsg.retained = 0;
     MQTTClient_deliveryToken token;
     MQTTClient_publishMessage(client, topic, &pubmsg, &token);
     MQTTClient_waitForCompletion(client, token, 1000L);

     printf("Mensagem enviada! \n\rTopico: %s Mensagem: %s\n", topic, payload);
}

// RECEIVE MESSAGE
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);
    evaluateRecData(topicName, payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

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
           char inputValue[] = "";
           strncpy(inputValue, payload + 2, sizeof(payload) - 2);       // COPYING ONLY THE INPUT VALUE RECEIVED
           
           printf("Resposta 01: %s. \n", inputValue);
        }

        //02
        else if (payload[0] == '0' && payload[1] == '2'){
           char digitalSensor[] = "";
           char inputValue[] = "";
           strncpy(digitalSensor, payload + 2, sizeof(payload) - 2);       // COPYING ONLY THE DIGITAL SENSOR RECEIVED
           strncpy(inputValue, payload + 4, sizeof(payload) - 4);       // COPYING ONLY THE INPUT VALUE RECEIVED
           
           printf("Resposta 02: %s , %s. \n", digitalSensor, inputValue);
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
