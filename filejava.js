var host = "broker.emqx.io"
var port = 8083
var configTopic = "test/configMotor"
// var userTopic =   "test/userRequest"
// client = new Paho.MQTT.Client(host , port , "TTH_IOT_Config");
client = new Paho.MQTT.Client(host , port , "huy");


client.destinationName = configTopic;
// client.destinationName1 = userTopic;
client.qos = 1;
client.retained = true;

// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

// // set callback handlers
// client.onConnectionLost = onConnectionLost1;
// client.onMessageArrived = onMessageArrived1;
// connect the client
client.connect({onSuccess:onConnect});
// client.connect({onSuccess:onConnect1});


// called when the client connects
function onConnect() {
  // Once a connection has been made, make a subscription and send a message.
  console.log("onConnectConfig");
  client.subscribe(configTopic);  
}
// // called when the client connects
// function onConnect1() {
//   // Once a connection has been made, make a subscription and send a message.
//   console.log("onConnectUser");
//   client.subscribe(userTopic);  
// }

// Start , Stop 
function publish_Start(){
    var selectedValue = document.getElementById("list").value;

    message = new Paho.MQTT.Message(selectedValue);
    message.destinationName = configTopic;
    message.qos = 1;
    client.send(message); 
}

function publish_Stop(){
    message = new Paho.MQTT.Message("0");
    message.destinationName = configTopic;
    message.qos = 1;
    client.send(message); 
}
function user_1(){
  message = new Paho.MQTT.Message("11");
  message.destinationName = configTopic;
  message.qos = 1;
  client.send(message); 
}
function user_2(){
    message = new Paho.MQTT.Message("22");
    message.destinationName = configTopic;
    message.qos = 1;
    client.send(message); 
}
function pressDone(){
    message = new Paho.MQTT.Message("33");
    message.destinationName = configTopic;
    message.qos = 1;
    client.send(message); 
}
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
      console.log("onConnectionLost:"+responseObject.errorMessage);
    }
}

  // called when a message Config arrives
function onMessageArrived(message) {
  console.log("ConfigMessageArrived:"+message.payloadString);
}
// // called when a message User arrives
// function onMessageArrived1(message1) {
//   console.log("UserMessageArrived:"+message1.payloadString);
// }