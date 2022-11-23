//gcc mqtt.c -o mqtt.run -lpaho-mqtt3c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>

#define MQTT_ADDRESS   "tcp://10.0.0.101:1883"
#define USER           "aluno"
#define PASSWORD       "@luno*123"
#define CLIENTID       "MQTTCClientRASP"  
#define QOS            1
#define TIMEOUT        100000L

#define NODEMCU_TOPIC  "nodemcu"

MQTTClient client;

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

int main(int argc, char *argv[])
{
   int rc;
   MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
   MQTTClient_message pubmsg = MQTTClient_message_initializer;
   MQTTClient_deliveryToken token;
   
   MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
   conn_opts.keepAliveInterval = 20;
   conn_opts.cleansession = 1;
   conn_opts.username = USER;
   conn_opts.password = PASSWORD;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
   
   char *messageRec = "atumalacakkkk";


  // Publish message
    pubmsg.payload = messageRec;
    pubmsg.payloadlen = strlen(messageRec);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, NODEMCU_TOPIC, &pubmsg, &token);
    printf("Waiting for up to %d seconds for publication of %s\n on topic %s for client with ClientID: %s\n",(int)(TIMEOUT/1000), messageRec, NODEMCU_TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
  
    // Disconnect
    //MQTTClient_disconnect(client, 10000);
    //MQTTClient_destroy(&client);
    return rc;
}

//SEND MESSAGE
void publish(MQTTClient client, char* topic, char* payload) {
     printf("Mensagem enviada! \n\rTopico: %s Mensagem: %s\n", topic, payload);
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}

// RECEIVE MESSAGE
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);

    ///* Faz echo da mensagem recebida */
    publish(client, NODEMCU_TOPIC, payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
