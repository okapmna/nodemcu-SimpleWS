#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <DHT.h>

// Konfigurasi WiFi
const char* ssid = "NYUDIS";
const char* password = "87654321C";

// Konfigurasi DHT11
#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define RELAYPIN_1 D4

// Inisialisasi WebSocket Server
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(9600);
  dht.begin();


  // Hubungkan ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi...");
  }
  Serial.println("WiFi Terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Mulai WebSocket Server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  // Baca suhu setiap 2 detik
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 2000) {
    lastTime = millis();
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      String tempStr = String(temperature);
      webSocket.broadcastTXT(tempStr); // Kirim data suhu ke semua klien
      Serial.println("Suhu: " + tempStr + " °C");
    } else {
      Serial.println("Gagal membaca data dari sensor!");
    }
  }
}

// Fungsi untuk menangani event WebSocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.printf("[%u] Pesan diterima: %s\n", num, payload);
    webSocket.sendTXT(num, "Pesan diterima!");
  }
}
