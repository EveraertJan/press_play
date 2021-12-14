
//Wifi library
#include <WiFi.h>
//MQTT library
#include <PubSubClient.h>

//Network details, update for your own need
const char* ssid = "wifi";
const char* password = "passwoord";

/*--------MQTT section-------------------*/
//MQTT server details
const char* mqtt_server = "192.168.178.184 ";

WiFiClient espClient;
PubSubClient client(espClient);
//timerHelp
unsigned long lastMsg = 0;
//MQTT message buffer
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
//MQTT topics
String inComming;
String outtopicStatus;
//messageCounter
int value = 0;

//ESP control library needed for restart
#include "esp_system.h"

//MACadress global values
char* mychar;
String s;
int slen;
String outtopic;


#include <EasyButton.h>
#define BUTTON_ONE_PIN 14
#define BUTTON_TWO_PIN 27

// Button1
EasyButton button1(BUTTON_ONE_PIN);
// Button2
EasyButton button2(BUTTON_TWO_PIN);

bool button1State = false;
bool button2State = false;

// Callback function to be called when button1 is pressed
void onButton1Pressed() {
  Serial.println("Button1 pressed");
  button1State = !button1State;
 // snprintf (msg, 75, "controllers,id=%s button=1", mychar);
  client.publish("0fba8e96-8560-462b-bc66-93ca3e7bc5f5", "{\"test\"=\"test\"}");
}

// Callback function to be called when button2 is pressed
void onButton2Pressed() {
  Serial.println("Button2 pressed");
  button2State = !button2State;
  snprintf (msg, 75, "controllers,id=%s button=2", mychar);
  client.publish((char*)outtopic.c_str(), msg);
}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

    if (millis() / 1000 > 3) {
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //saving MACadress to global
  s = WiFi.macAddress(); // Use Mac ID as a mychar
  slen = s.length();
  mychar = &s[0];
  outtopic = "controllers/" + s + "/input";
  outtopicStatus = "controllers/" + s + "/status";



}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  inComming = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    inComming += (char)payload[i];
  }
  Serial.println();
  Serial.println(inComming);
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(12, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(12, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "0fba8e96-8560-462b-bc66-93ca3e7bc5f5";
   
    // Attempt to connect
    if (client.connect(clientId.c_str(),"0fba8e96-8560-462b-bc66-93ca3e7bc5f5","HtcX5YRWIDVLsWMUQlLZ8QvkYNbDznzRALI2fiW6")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("controllers/lightshow");
      client.subscribe("controllers/mode");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void haertbeat() {
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    snprintf (msg, 75, "controllers,id=%s time=%ld", mychar, now / 1000);
    client.publish((char*)outtopicStatus.c_str(), msg);
  }
  
}
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(25, OUTPUT);
  pinMode(12, OUTPUT);

  button1.begin();
  // Initialize the button2
  button2.begin();
  // Add the callback function to be called when the button1 is pressed.
  button1.onPressed(onButton1Pressed);
  // Add the callback function to be called when the button2 is pressed.
  button2.onPressed(onButton2Pressed);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}



// the loop function runs over and over again forever
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Continuously read the status of the buttons
  button1.read();
  button2.read();


  haertbeat();

  

  if (button1State == true) {
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);
  }

  if (button2State == true) {
    digitalWrite(25, HIGH);
  } else {
    digitalWrite(25, LOW);
  }

}
