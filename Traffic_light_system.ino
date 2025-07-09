#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define DATA_PIN 11
#define CS_PIN   10
#define CLK_PIN  13

MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Ultrasonic sensor pins
#define TRIG_PIN 8
#define ECHO_PIN 9

bool isScrolling = false;
bool objectDetected = false;  // To prevent retrigger while object stays
unsigned long lastTriggerTime = 0;

void setup() {
  display.begin();
  display.setIntensity(2);
  display.setTextAlignment(PA_LEFT);
  display.setSpeed(40);
  display.setPause(0);
  display.displayClear();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  long distance = getDistance();

  // Detect object entering zone
  if (distance > 0 && distance < 30 && !isScrolling && !objectDetected) {
    display.displayClear();
    display.displayScroll("Welcome to XII-Science", PA_LEFT, PA_SCROLL_LEFT, 40);
    isScrolling = true;
    objectDetected = true;
    lastTriggerTime = millis();
  }

  // Reset objectDetected when object moves away
  if (distance > 37 && objectDetected) {
    objectDetected = false;
  }

  // Animate the scroll
  if (isScrolling) {
    if (display.displayAnimate()) {
      isScrolling = false;
      display.displayClear();
    }
  }
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000);  // 20ms timeout
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}
