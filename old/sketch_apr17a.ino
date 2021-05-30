#include <MeMCore.h>
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

MeRGBLed rgbled_7(7, 2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.flush();
  rgbled_7.fillPixelsBak(0, 2, 1);
  rgbled_7.setColor(1, 52, 79);
  rgbled_7.show();

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.readStringUntil('`') == "RGB") {
    rgbled_7.setColor(0, Serial.readStringUntil('`').toInt(), Serial.readStringUntil('`').toInt(), Serial.readStringUntil('`').toInt());
    rgbled_7.show();
    Serial.println("Command recieved by mBot");
    Serial.flush();
  }
  if (Serial.readStringUntil('`') == "Hello") {
    rgbled_7.setColor(0,255,255,255);
    rgbled_7.show();
    Serial.println("Hello");
  }
  delay(100);

}
