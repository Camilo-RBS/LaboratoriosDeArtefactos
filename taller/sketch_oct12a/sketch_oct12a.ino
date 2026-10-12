#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h> // Librería necesaria para el sensor DHT11

// ======================================================
// CONFIGURACIÓN WIFI
// ======================================================

// RECUERDA PONER LOS DATOS DE TU RED AQUÍ
const char* WIFI_SSID = "Alejo";
const char* WIFI_PASSWORD = "pupusa123";


// ======================================================
// CONFIGURACIÓN MQTT
// ======================================================

const char* MQTT_SERVER = "192.168.43.207"; // Tu IP
const int MQTT_PORT = 1883;


// Topics utilizados por el dashboard React.
const char* TOPIC_TEMPERATURA = "clima/esp32/temperatura";
const char* TOPIC_HUMEDAD = "clima/esp32/humedad";
const char* TOPIC_LUZ = "clima/esp32/luz";


// ======================================================
// CONFIGURACIÓN DE SENSORES
// ======================================================

// Definir los pines para DHT11 y LDR
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LDR_PIN 32

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Enviar una lectura cada 2 segundos.
const unsigned long INTERVALO_ENVIO = 2000;
unsigned long ultimoEnvio = 0;

void conectarWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando a Mosquitto...");
    // Crear un Client ID diferente para este ESP32.
    String clientId = "ESP32-";
    clientId += String((uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println(" conectado");
    } else {
      Serial.print(" error, rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Reintentando en 2 segundos...");
      delay(2000);
    }
  }
}

// ======================================================
// LEER Y PUBLICAR SENSORES
// ======================================================

void publicarSensores() {
  // 1. Obtener la temperatura
  float t = dht.readTemperature();
  // 2. Obtener la humedad
  float h = dht.readHumidity();
  
  // Comprobar si la lectura falló
  if (isnan(t) || isnan(h)) {
    Serial.println("Error al leer el sensor DHT11!");
    return;
  }

  // 3. Obtener la lectura del sensor de iluminación (valores de 0 a 4095 en ESP32)
  int valorLuz = analogRead(LDR_PIN);

  // 4. Preparar los valores para publicarlos mediante MQTT (convertirlos a texto)
  char tempStr[8];
  char humStr[8];
  char luzStr[8];
  
  dtostrf(t, 1, 2, tempStr);
  dtostrf(h, 1, 2, humStr);
  itoa(valorLuz, luzStr, 10);

  // 5. Publicar cada dato en su topic
  mqttClient.publish(TOPIC_TEMPERATURA, tempStr);
  mqttClient.publish(TOPIC_HUMEDAD, humStr);
  mqttClient.publish(TOPIC_LUZ, luzStr);

  // 6. Mostrar las mediciones en el Monitor Serial
  Serial.print("Temperatura: ");
  Serial.print(tempStr);
  Serial.print(" °C | Humedad: ");
  Serial.print(humStr);
  Serial.print(" % | Luz (ADC): ");
  Serial.println(luzStr);
}

void setup() {
  Serial.begin(115200);

  // Inicializar los sensores
  dht.begin();
  pinMode(LDR_PIN, INPUT);

  conectarWiFi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();

  if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {
    ultimoEnvio = millis();
    publicarSensores();
  }
}