#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LM35_PIN A0

void setup() {
  Serial.begin(9600);
  Wire.begin();

  delay(250);

  // Inicializar pantalla OLED
  if (!display.begin(i2c_Address, true)) {
    Serial.println("ERROR: OLED no encontrada");
    while (1);
  }

  Serial.println("OLED y LM35 funcionando correctamente");
}

void loop() {
  // Valor
  int lecturaActual = analogRead(LM35_PIN);

  // Caculo de la temperatura
  float temperatura = (lecturaActual * 5.0 / 1023.0) * 100.0;

  Serial.print("Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println(" °C");

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);


  display.setTextSize(1);
  display.setCursor(15, 5);
  display.println("TEMPERATURA (LM35)");

  // valor de la temperatura
  display.setTextSize(2);
  display.setCursor(20, 28);
  display.print(temperatura, 2);
  display.print(" ");
  display.setTextSize(1);
  display.println("C");

  // Actualizar pantalla
  display.display();
  delay(1000);
}