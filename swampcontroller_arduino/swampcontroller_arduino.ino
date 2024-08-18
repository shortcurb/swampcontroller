#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "secrets.h"
// secrets.h includes wifi credentials, MQTT credentials, plus the subscribe topics and publish topics
#define HEARTBEAT_INTERVAL 6000 // 6 seconds

const int fantriggerpin = 14; 
const int fanselectpin = 12; 
const int pumptriggerpin = 13; 

const unsigned long heartbeatInterval = 5000;
unsigned long lastHeartbeat = 0;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, mqttserv, mqttport, mqttuser, mqttpass);

/****************************** Feeds ***************************************/
Adafruit_MQTT_Subscribe swampcommandfeed = Adafruit_MQTT_Subscribe(&mqtt, mqttuser swampcommandfeedstr);
Adafruit_MQTT_Publish swampresponsefeed = Adafruit_MQTT_Publish(&mqtt, mqttuser swampresponsefeedstr);
Adafruit_MQTT_Publish swampstatefeed = Adafruit_MQTT_Publish(&mqtt, mqttuser swampstatefeed);

/*************************** Sketch Code ************************************/

unsigned long lastReconnectAttempt = 0;

void setup() {

  // Need to set pins low when first booting. Low is relay deactivated.
  // Otherwise, if the wifi was down, your swamp cooler would turn on and remain on forever!!
  Serial.println("Setting pins");
  pinMode(fantriggerpin, OUTPUT);
  pinMode(fanselectpin, OUTPUT);
  pinMode(pumptriggerpin, OUTPUT);
  digitalWrite(fantriggerpin,LOW);
  digitalWrite(fanselectpin,LOW);
  digitalWrite(pumptriggerpin,LOW);

  Serial.begin(115200);
  delay(10);
  Serial.println();
  
  connectToWiFi();
    
  Serial.println("Subscribing to feeds");
  mqtt.subscribe(&swampcommandfeed);

  Serial.println("Setup complete");
}

void loop() {
  // Ensure we're connected to WiFi and the MQTT server
  ensureConnected();

  // Read subscription
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &swampcommandfeed) {
      Serial.print(F("Got: "));
      Serial.println((char *)swampcommandfeed.lastread);

      int pins[] = {-1,-1,-1};
      if      (strcmp((char*)swampcommandfeed.lastread, "SWAMP_HIGH") == 0){pins[0]=1; pins[1] = 1; pins[2] = 1;}
      else if (strcmp((char*)swampcommandfeed.lastread, "SWAMP_LOW")  == 0){pins[0]=1; pins[1] = 1; pins[2] = 0;}
      else if (strcmp((char*)swampcommandfeed.lastread, "FAN_HIGH")   == 0){pins[0]=0; pins[1] = 1; pins[2] = 1;}  
      else if (strcmp((char*)swampcommandfeed.lastread, "FAN_LOW")    == 0){pins[0]=0; pins[1] = 1; pins[2] = 0;}
      else if (strcmp((char*)swampcommandfeed.lastread, "PUMP_ON")    == 0){pins[0]=1; pins[1] = 0; pins[2] = -1;}
      else if (strcmp((char*)swampcommandfeed.lastread, "OFF")        == 0){pins[0]=0; pins[1] = 0; pins[2] = -1;}

      switchRelays(pins);
      char response[100];
      sprintf(response, "pump:%d, fan trigger:%d, fan select:%d", pins[0], pins[1], pins[2]);
      swampresponsefeed.publish(response);
      sendHeartbeat();
      mqtt.disconnect();
      delay(5000);
    }
  }

  // Send heartbeat at regular intervals
  unsigned long now = millis();
  if (now - lastHeartbeat > HEARTBEAT_INTERVAL) {
    lastHeartbeat = now;
    sendHeartbeat();
  }
}

void sendHeartbeat() {
  unsigned long currentMillis = millis();
  char heartbeatMsg[100];
  const char* pinState = readPins();
  snprintf(heartbeatMsg, sizeof(heartbeatMsg), "{\"Current State\": \"%s\", \"Heartbeat\": %lu}", pinState, currentMillis);
  Serial.println(heartbeatMsg);
  swampstatefeed.publish(heartbeatMsg);
}

const char* readPins() {
  static char currentstate[20]; // Static to persist the state outside this function
  int pin1 = digitalRead(pumptriggerpin);
  int pin2 = digitalRead(fantriggerpin);
  int pin3 = digitalRead(fanselectpin);
  
  String pinstates = String(pin1) + String(pin2) + String(pin3);
  
  if      (pinstates == "111") { strcpy(currentstate, "SWAMP_HIGH"); }
  else if (pinstates == "110") { strcpy(currentstate, "SWAMP_LOW"); }
  else if (pinstates == "011") { strcpy(currentstate, "FAN_HIGH"); }
  else if (pinstates == "010") { strcpy(currentstate, "FAN_LOW"); }
  else if (pinstates == "101" || pinstates == "100") { strcpy(currentstate, "PUMP_ON"); }
  else if (pinstates == "001" || pinstates == "000") { strcpy(currentstate, "OFF"); }
  else { strcpy(currentstate, "UNKNOWN"); }
  
  return currentstate;
}


void switchRelays(int pins[]){
  if (pins[0] == 1 || pins[0] == 0){digitalWrite(pumptriggerpin,pins[0]);}
  if (pins[1] == 1 || pins[1] == 0){digitalWrite(fantriggerpin,pins[1]);}
  if (pins[2] == 1 || pins[2] == 0){digitalWrite(fanselectpin,pins[2]);}
}


void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    yield();  // Reset the watchdog timer
  }
  Serial.println("MQTT connected");
}


void ensureConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
  
  if (!mqtt.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (mqtt.connect()) {
        lastReconnectAttempt = 0;
      }
    }
  }
}


void connectToWiFi() {
  // Check if the ESP is already connected to WiFi
  if (WiFi.status() == WL_CONNECTED) {
    return; // Already connected, so exit the function
  }
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, wifipass);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
