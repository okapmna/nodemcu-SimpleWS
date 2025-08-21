#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>
#include <DHT.h>
#include <ArduinoJson.h>



const char* ssidAP = "nodemcuAP";
const char* passwordAP = "nodemcuPASS";

#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define RELAYPIN_1 D3


WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(9600);
  dht.begin();


  //Declare Relay PIN
  pinMode(RELAYPIN_1, OUTPUT);
  digitalWrite(RELAYPIN_1, LOW);


  // WiFi Connection
  WiFi.softAP(ssidAP, passwordAP);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Start
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  static unsigned long lastTime = 0;

  if (millis() - lastTime > 500) {
    lastTime = millis();

    //DHT sensor
    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature();
    float tempF = dht.readTemperature(true);

    //RELAY status
    bool relayStatus = digitalRead(RELAYPIN_1);


    if (!isnan(tempC) && !isnan(tempC) && !isnan(humidity)) {

      StaticJsonDocument<200> doc;
      doc["tempC"] = tempC;
      doc["tempF"] = tempF;
      doc["humidity"] = humidity;
      doc["relayStatus"] = relayStatus;



      String espString;
      serializeJson(doc, espString);

      webSocket.broadcastTXT(espString);  // Broadcast all client
      Serial.println("Data send: " + espString);
      Serial.println(relayStatus);
    } else {
      Serial.println("Failed to read sensor!");
    }
  }
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.printf("[%u] Pesan diterima: %s\n", num, payload);
    String message = String((char*)payload);
    if (message == "ON") {
      digitalWrite(RELAYPIN_1, HIGH);
      //webSocket.sendTXT(num, "Relay ON");
    } else {
      digitalWrite(RELAYPIN_1, LOW);
      //webSocket.sendTXT(num, "Relay OFF");
    }
  }
}