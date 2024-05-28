#include <Arduino.h>
#include <driver/i2s.h>
#include <hal/i2s_types.h>


void setup(){
  Serial.begin(115200);
  Serial.println("com setup ok!");
}

void loop(){
  Serial.println("output from Serial");
  delay(1000);
}