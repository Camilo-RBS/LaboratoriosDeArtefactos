void setup(){

  Serial.begin(115200);

  delay(1000);
  
  Serial.println("======================");
  Serial.println("00083223 - CamiloBenitez");
  Serial.println("======================");
  Serial.println();

}

void loop(){
  Serial.print("ESP32 funcionando correctamente - tiempo activa: ");
  Serial.print(millis()/1000.0);
  Serial.println("Segundos");

  delay(2000);

}