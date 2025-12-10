#include <Arduino.h>
#define BUTTON_PIN 4   // ขาปุ่มที่ต่อไว้ (Active-Low)
int count = 0;
int lastState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // ใช้ Pull-up ภายใน
  Serial.println("Example 1: Polling button without PCNT");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);
  // ตรวจจับ "ขาลง" (กดลงจาก HIGH -> LOW)
  if (lastState == HIGH && reading == LOW) {
    count++;
    Serial.print("Count = ");
    Serial.println(count);
  }
  lastState = reading;
  //delay(10); // หน่วงเล็กน้อยเพื่อลดการอ่านถี่เกินไป
}
