const mqtt = require('mqtt');

// CONFIGURAÇÕES BROKER LABORATÓRIO LEDS
const host = "mqtt://10.0.0.101";
const options = {
  port: 1883,
  clientId:"mqtt_pbl3SD",
  username: "aluno",
  password: "@luno*123",
};

const client = mqtt.connect(host, options);
//const client = mqtt.connect("mqtt://broker.emqx.io", 1883);

const historico = "{'MARIA':'[270,236,220,200,289,275,100,23]','timestamps':'[410811,412551,412580,412702,413003,413102,413122,413202]'}"

const topic_history1 = "SENSORS_HISTORY/D2";
const topic_subscribe = "nodeEnvia";


client.on("connect",()=>{
    console.log('client connect');
    client.subscribe(topic_history1);
        client.publish(topic_history1,JSON.stringify("oia")) 
})

client.on("message",(topic,msg)=>{
    console.log(msg.toString());
})
