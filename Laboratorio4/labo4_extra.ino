#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SH1106G display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

const int TOUCH_PIN_1 = 4;
const int TOUCH_PIN_2 = 2;

const int UMBRAL_TOUCH = 950;

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!display.begin(OLED_ADDRESS, true)) {
    Serial.println("Error al iniciar la pantalla OLED");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
}

void loop() {
  // Lectura de ambos sensores
  int valorTouch1 = touchRead(TOUCH_PIN_1);
  int valorTouch2 = touchRead(TOUCH_PIN_2);

  bool tocando1 = (valorTouch1 <= UMBRAL_TOUCH);
  bool tocando2 = (valorTouch2 <= UMBRAL_TOUCH);

  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(26, 5);
  display.println("ESP32 TOUCH");

  display.setCursor(10, 25);
  if (tocando1) {
    display.println("L tocando");
    display.fillCircle(25, 48, 5, SH110X_WHITE);
  } else {
    display.println("L");
    display.drawCircle(25, 48, 5, SH110X_WHITE);
  }


  display.setCursor(70, 25);
  if (tocando2) {
    display.println("R tocando");
    display.fillCircle(85, 48, 5, SH110X_WHITE);
  } else {
    display.println("R");
    display.drawCircle(85, 48, 5, SH110X_WHITE);
  }

  display.display();
  delay(100);
}