#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif

// topics define
#define TOPICO_SUBSCRIBE "NODEMCU_PUBLISH"
#define TOPICO_PUBLISH "NODEMCU_RECEIVE"
#define ID_MQTT "SDPBL3"
#define TOPICO_SUBSCRIBE2 "nodemcu"
#define IHM_TIMECHANGE "IHM_TIMECHANGE"
#define BUFFER_SIZE 64
//response command
String operating_normally   = "00";
String with_problem         = "1F";
String analog_entry_measure = "01";
String digital_input_status = "02";

String COMANDO;
int delay_time_change = 1000;

//String comand = "";
const char* ssid = STASSID;
const char* password = STAPSK;

// Nome do ESP na rede
const char* host = "ESP-10.0.0.108";

// Definições de rede
IPAddress local_IP(10, 0, 0, 108);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 0, 0);

//settings broker mqtt
const char* mqqt_broker= "10.0.0.101";
const char* mqtt_user= "aluno";
const char* mqtt_password="@luno*123";

//Entradas Digitais
const int sensor_zero = D0;
const int sensor_D1 = D1;
const int sensor_D2 = D2;
const int sensor_D3 = D3;
const int sensor_D4 = D4;
const int sensor_D5 = D5;
const int sensor_D6 = D6;
const int sensor_D7 = D7;
const int sensor_D8 = D8;

// client wifi
WiFiClient espClient;
PubSubClient client(espClient);


// setup WIFI
void setup_wifi(){
  delay(20);
  Serial.println();
  Serial.print("Connecting to");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}



// callback function
void callback(const MQTT::Publish& pub){
 // Serial.print(pub.topic());
   //Serial.println(pub.payload_string());
  //Serial.print(" => ");
 
  if(pub.has_stream()){
    uint8_t buf[BUFFER_SIZE];
    int read;
    while(read = pub.payload_stream()->read(buf,BUFFER_SIZE)){
      Serial.write(buf,read);
    }
    pub.payload_stream()->stop();
    Serial.println("");
  }else{
      COMANDO = pub.payload_string();  // Recebe o valor DO BROKER
      Serial.println(pub.payload_string());
  }

}

//initial settings MQTT
void initMqtt(){

  client.set_server(mqqt_broker,1883);
//client.set_callback(mqtt_callback);
  client.set_callback(callback);
  if(client.connect(MQTT::Connect(ID_MQTT).set_auth(mqtt_user,mqtt_password))){
    Serial.println("Node conectada");
    client.subscribe(TOPICO_SUBSCRIBE);
    client.subscribe(TOPICO_PUBLISH);
    client.subscribe(IHM_TIMECHANGE);

  }else{
    
    }
  
}

void EnviaEstadoOutputMQTT(void){
 
    if(COMANDO == "30"){
      client.publish(TOPICO_PUBLISH,operating_normally);      
    }

    else if(COMANDO == "40"){
      String entrada = String(analogRead(A0));
      client.publish(TOPICO_PUBLISH,analog_entry_measure +entrada);
    }
    
    else if(COMANDO =="50"){
      String entrada_D0 = String(digitalRead(sensor_zero));
      String sensor_D0 = "D0";
      client.publish(TOPICO_PUBLISH,digital_input_status+sensor_D0+entrada_D0);          
    }
    
    else if(COMANDO == "51"){
      String entrada_D1 = String(digitalRead(sensor_D1));
      String sensor_name_D1 = "D1";
      client.publish(TOPICO_PUBLISH,digital_input_status+sensor_name_D1+entrada_D1);      
    }
    
    else if(COMANDO == "52"){
      String sensor_name_D2 = "D2";
      String entrada_D2 = String(digitalRead(sensor_D2));
      client.publish(TOPICO_PUBLISH,digital_input_status+sensor_name_D2+entrada_D2);
    }
    
    else if(COMANDO == "53"){
      String sensor_name_D3 = "D3";
      String entrada_D3 = String(digitalRead(sensor_D3));
      client.publish(TOPICO_PUBLISH,digital_input_status+sensor_name_D3+entrada_D3);     
    }
    
    else if(COMANDO == "54"){
      String sensor_name_D4="D4";
      String entrada_D4 = String(digitalRead(sensor_D4));
      client.publish(TOPICO_PUBLISH,digital_input_status+sensor_name_D4+entrada_D4); 
    }

    else if(COMANDO =="55"){
      String sensor_name_D5 = "D5";
       String entrada_D5 = String(digitalRead(sensor_D5));
       client.publish(TOPICO_PUBLISH,digital_input_status+sensor_name_D5+entrada_D5);
    }
    
    else if(COMANDO =="56"){
      String sensor_name_D6 ="D6";
      String entrada_D6 = String(digitalRead(sensor_D6));
      client.publish(TOPICO_PUBLISH,digital_input_status+sensor_name_D6+entrada_D6);           
    }
    
    else if(COMANDO == "57"){
      String sensor_name_D7="D7";
      String entrada_D7 = String(digitalRead(sensor_D7));
      client.publish(TOPICO_PUBLISH,digital_input_status+sensor_name_D7+entrada_D7);
    }
    
    else if(COMANDO == "58"){
      String sensor_name_D8="D8";
      String entrada_D8= String(digitalRead(sensor_D8));
      client.publish(TOPICO_PUBLISH,digital_input_status+sensor_name_D8+entrada_D8);      
    }
    
    else if(COMANDO=="60"){
      digitalWrite(LED_BUILTIN,LOW);
      client.publish(TOPICO_PUBLISH,operating_normally);
    }
    
    else if(COMANDO == "70"){
      digitalWrite(LED_BUILTIN,HIGH);
      client.publish(TOPICO_PUBLISH,operating_normally);
    }
    
    else{
      delay_time_change = atoi(COMANDO);
      if(delay_time_change > 10000){ delay_time_change = 3000; }
    }

   COMANDO="0";
}


void setup() {
  Serial.begin(115200);
  //Serial.println("Booting");

  // Configuração do IP fixo no roteador, se não conectado, imprime mensagem de falha
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname(host);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  // Setup project
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(sensor_zero,OUTPUT);
  pinMode(sensor_D1,OUTPUT);
  pinMode(sensor_D2,OUTPUT);
  pinMode(sensor_D3,OUTPUT);
  pinMode(sensor_D4,OUTPUT);
  pinMode(sensor_D5,OUTPUT);
  pinMode(sensor_D6,OUTPUT);
  pinMode(sensor_D7,OUTPUT);
  pinMode(sensor_D8,OUTPUT);  
  Serial.begin(9600);
  setup_wifi();
  initMqtt();
}

void loop() {
  ArduinoOTA.handle();
  EnviaEstadoOutputMQTT();  // Envia um dado para quem solicita
  client.loop();
  delay(delay_time_change);
}