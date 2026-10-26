#include <SPI.h>
#include <LoRa.h>

// ======================================================
// IDENTIFICACION
// ======================================================

const int MI_ID = 3;
const int DESTINO_ID = 2;

String mensajeOriginal = "SENSOR ACTIVO";   // cambie por su mensaje

// ======================================================
// PINES
// ======================================================

// Modulo LoRa RFM96W (SPI)
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_NSS   5
#define LORA_RST   -1   // no conectado
#define LORA_DIO0  -1   // no conectado

// Sensor Touch
#define PIN_TOUCH     27
#define UMBRAL_TOUCH  500   // ajustar segun los valores de su ESP32

// Variable de estado: evita enviar varios paquetes
// mientras el dedo sigue sobre el sensor
bool tocando = false;

// ======================================================
// ATBASH
// ======================================================

char convertirAtbash(char letra) {

  if (letra >= 'A' && letra <= 'Z') {
    return 'Z' - (letra - 'A');
  }

  if (letra >= 'a' && letra <= 'z') {
    return 'z' - (letra - 'a');
  }

  return letra;
}

String cifrarAtbash(String mensaje) {

  String mensajeCifrado = "";

  for (int i = 0; i < mensaje.length(); i++) {
    mensajeCifrado += convertirAtbash(mensaje.charAt(i));
  }

  return mensajeCifrado;
}

// ======================================================
// ENVIAR MENSAJE
// ======================================================

void enviarMensaje() {

  String mensajeCifrado = cifrarAtbash(mensajeOriginal);

  Serial.println();
  Serial.println("TOUCH DETECTADO");

  Serial.print("Mensaje original: ");
  Serial.println(mensajeOriginal);

  Serial.print("Mensaje cifrado: ");
  Serial.println(mensajeCifrado);

  // Formato del paquete: ORIGEN|DESTINO|MENSAJE
  LoRa.beginPacket();
  LoRa.print(MI_ID);
  LoRa.print("|");
  LoRa.print(DESTINO_ID);
  LoRa.print("|");
  LoRa.print(mensajeCifrado);
  LoRa.endPacket();

  Serial.print("Paquete enviado: ");
  Serial.print(MI_ID);
  Serial.print("|");
  Serial.print(DESTINO_ID);
  Serial.print("|");
  Serial.println(mensajeCifrado);

  Serial.println("----------------------------");
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("TRANSMISOR LORA");

  Serial.print("Mi ID: ");
  Serial.println(MI_ID);

  Serial.print("Destino: ");
  Serial.println(DESTINO_ID);

  // Iniciar el bus SPI: SCK, MISO, MOSI, SS
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);

  // Indicar a la libreria los pines del modulo: NSS, RESET, DIO0
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(915E6)) {
    Serial.println("ERROR: No se pudo iniciar LoRa");
    while (true);
  }

  Serial.println("LoRa iniciado correctamente");
  Serial.println("Esperando toque...");
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  // Leer el sensor Touch
  int valorTouch = touchRead(PIN_TOUCH);


  // Serial.print("Touch: ");
  // Serial.println(valorTouch);

  if (valorTouch < UMBRAL_TOUCH) {
    // Solo se envia en el momento en que empieza el toque
    if (!tocando) {
      tocando = true;
      enviarMensaje();
    }
  } else {
    // El dedo se retiro: se habilita el siguiente envio
    tocando = false;
  }

  delay(100);
}