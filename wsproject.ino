#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <DHT.h>
#include <ArduinoJson.h>



const char* ssid = "NYUDIS";
const char* password = "87654321C";

#define DHTPIN D2
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
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connect to WiFi...");
  }

  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

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

      webSocket.broadcastTXT(espString); // Broadcast all client
      Serial.println("Data send: " + espString);
    } else {
      Serial.println("Gagal membaca data dari sensor!");
    }
  }
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
   if (type == WStype_TEXT) {
     Serial.printf("[%u] Pesan diterima: %s\n", num, payload);
     String message = String((char*)payload);
     if(message == "ON"){
      digitalWrite(RELAYPIN_1, HIGH);
      //webSocket.sendTXT(num, "Relay ON");
     } 
     else {
      digitalWrite(RELAYPIN_1, LOW);
      //webSocket.sendTXT(num, "Relay OFF");
     }
   }
}