const { readFile, writeFile } = require("fs/promises");
const bodyParser = require("body-parser");
const { WriteFile, ReadFile } = require("./service");
const mqtt = require("mqtt");
const express = require("express");
const cors = require("cors");

//CONFIGURAÇÃO SERVIDOR HTTP
const server = express();
server.use(
  cors({
    origin: "*",
    methods: "GET,HEAD,PUT,PATCH,POST,DELETE",
    preflightContinue: false,
    optionsSuccessStatus: 204,
    credentials: true,
  })
);
server.use(bodyParser.urlencoded({ extended: false }));
server.use(bodyParser.json());
const PORT = 3000;

// CONFIGURAÇÕES BROKER LABORATÓRIO LEDS
const host = "mqtt://10.0.0.101";
const options = {
  port: 1883,
  clientId: "mqtt_pbl3SD",
  username: "aluno",
  password: "@luno*123",
};

const client = mqtt.connect(host, options);

//const client = mqtt.connect("mqtt://broker.emqx.io", 1883);

// DEFINIÇÃO DOS TOPICOS
const topic_publish = "NODEMCU_PUBLISH";
const topic_history0 = "SENSORS_HISTORY/A0";
const topic_historyD0 = "SENSORS_HISTORY/D0";
const topic_historyD1 = "SENSORS_HISTORY/D1";
const topic_historyD2 = "SENSORS_HISTORY/D2";
const topic_historyD3 = "SENSORS_HISTORY/D3";
const topic_historyD4 = "SENSORS_HISTORY/D4";
const topic_historyD5 = "SENSORS_HISTORY/D5";
const topic_historyD6 = "SENSORS_HISTORY/D6";
const topic_historyD7 = "SENSORS_HISTORY/D7";
const topic_historyD8 = "SENSORS_HISTORY/D8";
const situacao_mcu = "STATUS_NODEMCU";

const topic_subscribe = "nodeEnvia";

let situacao_node;
//Evento de Conexao
client.on("connect", () => {
  console.log("client connect");
  client.subscribe(situacao_mcu);

  client.subscribe(topic_history0);
  client.subscribe(topic_historyD0);
  client.subscribe(topic_historyD1);
  client.subscribe(topic_historyD2);
  client.subscribe(topic_historyD3);
  client.subscribe(topic_historyD4);
  client.subscribe(topic_historyD5);
  client.subscribe(topic_historyD6);
  client.subscribe(topic_historyD7);
  client.subscribe(topic_historyD8);
});
//Escuta Mensagem
client.on("message", async (topico, msg) => {
  if(topico == situacao_mcu){
    
    situacao_node = msg.toString();
    console.log("situacao node: ",situacao_node)
  }else{

    const value_payload = msg.toString();
  
    const array_payload = JSON.parse(value_payload);
    const { historico, timestamps } = array_payload;
    console.log("Mensagem Recebida: ", array_payload);
  
    // WriteFileSensors(topico,JSON.parse( msg.toString()));
    WriteFileSensors(topico, array_payload);
  }
});

const EnviaMsg = (message) => {
  console.log(typeof message);
  client.publish(topic_publish, message);
};

// Ler Base de dados (arquivo Json)
const ReadFileSensors = async (path) => {
  return JSON.parse(await readFile(path));
};

//Escreve na Base de dados (arquivo Json)
const WriteFileSensors = async (topic, message) => {
  if (topic == topic_history0) {
    const path = "./data_bases/data_base_a0.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD0) {
    const path = "./data_bases/data_base_s0.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD1) {
    const path = "./data_bases/data_base_s1.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD2) {
    const path = "./data_bases/data_base_s2.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD3) {
    const path = "./data_bases/data_base_s3.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD4) {
    const path = "./data_bases/data_base_s4.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD5) {
    const path = "./data_bases/data_base_s5.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD6) {
    const path = "./data_bases/data_base_s6.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD7) {
    const path = "./data_bases/data_base_s7.json";
    WriteFile(path, message);
  } else if (topic == topic_historyD8) {
    const path = "./data_bases/data_base_s8.json";
    WriteFile(path, message);
  }

  return true;
};

// Rotas do Servidor

// Busca todos os dados
server.get("/sensores/:sensor", async (req, res) => {
  res.header("Access-Control-Allow-Origin", "*");
  const sensor = req.params.sensor;
  if (sensor == "a0") {
    const path = "./data_bases/data_base_a0.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d0") {
    const path = "./data_bases/data_base_s0.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d1") {
    const path = "./data_bases/data_base_s1.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d2") {
    const path = "./data_bases/data_base_s2.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d3") {
    const path = "./data_bases/data_base_s3.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d4") {
    const path = "./data_bases/data_base_s4.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d5") {
    const path = "./data_bases/data_base_s5.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d6") {
    const path = "./data_bases/data_base_s6.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d7") {
    const path = "./data_bases/data_base_s7.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  } else if (sensor == "d8") {
    const path = "./data_bases/data_base_s8.json";
    const sensorData = await ReadFileSensors(path);
    return res.send(sensorData);
  }
  return res.send("error");
});

server.get("/situacaoNode",(req,res)=>{
  return res.send(situacao_node);
})

//Envia Comando para Broker
server.post("/comando", (req, res) => {
  const { comando } = req.body;
  EnviaMsg(comando);
});

server.listen(PORT, () => {
  console.log("Server is running in Port: ", PORT);
});
