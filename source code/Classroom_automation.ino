#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <MFRC522.h>
#include <SPI.h>

char auth[] = "your_blynk_auth_token";
char ssid[] = "your_wifi_ssid";
char pass[] = "your_wifi_password";

BlynkTimer timer;

#define SS_PIN 5
#define RST_PIN 4
#define relay0_pin 12 
#define trigPin 2
#define echoPin 15
#define relay5_pin 22

#define button1_pin 34
#define button2_pin 35
#define button3_pin 32
#define button4_pin 33
#define button5_pin 13
#define button0_pin 21

#define relay1_pin 25
#define relay2_pin 26
#define relay3_pin 14
#define relay4_pin 27

int relay1_state = 0;
int relay2_state = 0;
int relay3_state = 0;
int relay4_state = 0;
int relay5_state = 0;
int relay0_state = 0;

#define button1_vpin    V1
#define button2_vpin    V2
#define button3_vpin    V3 
#define button4_vpin    V4
#define button5_vpin    V5
#define button0_vpin    V0

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);

  pinMode(button1_pin, INPUT_PULLUP);
  pinMode(button2_pin, INPUT_PULLUP);
  pinMode(button3_pin, INPUT_PULLUP);
  pinMode(button4_pin, INPUT_PULLUP);
  pinMode(button5_pin, INPUT_PULLUP);
  pinMode(button0_pin, INPUT_PULLUP);

  pinMode(relay1_pin, OUTPUT);
  pinMode(relay2_pin, OUTPUT);
  pinMode(relay3_pin, OUTPUT);
  pinMode(relay4_pin, OUTPUT);
  pinMode(relay5_pin, OUTPUT);
  pinMode(relay0_pin, OUTPUT);

  digitalWrite(relay1_pin, HIGH);
  digitalWrite(relay2_pin, HIGH);
  digitalWrite(relay3_pin, HIGH);
  digitalWrite(relay4_pin, HIGH);
  digitalWrite(relay5_pin, HIGH);
  digitalWrite(relay0_pin, HIGH);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, listen_push_buttons);

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  Blynk.run();
  timer.run();
  door();
  distance();
}

void door() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Card detected! Activating relay...");
    digitalWrite(relay0_pin, LOW);
    Blynk.virtualWrite(button0_vpin, 1);
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void distance() {
  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = (duration * 0.0343) / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 5) {
    digitalWrite(relay5_pin, LOW);
    Blynk.virtualWrite(button5_vpin, 1);
  } else {
    digitalWrite(relay5_pin, HIGH);
    Blynk.virtualWrite(button5_vpin, 0);
  }
}

void listen_push_buttons() {
  control_relay(1, button1_pin, button1_vpin, relay1_pin, relay1_state);
  control_relay(2, button2_pin, button2_vpin, relay2_pin, relay2_state);
  control_relay(3, button3_pin, button3_vpin, relay3_pin, relay3_state);
  control_relay(4, button4_pin, button4_vpin, relay4_pin, relay4_state);
  control_relay(5, button5_pin, button5_vpin, relay5_pin, relay5_state);
  control_relay(0, button0_pin, button0_vpin, relay0_pin, relay0_state);
}

void control_relay(int relay, int buttonPin, int virtualPin, int relayPin, int &relayState) {
  int buttonState = digitalRead(buttonPin);
  static bool lastButtonState = HIGH;
  static unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 50;

  if (buttonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState != relayState) {
      relayState = buttonState;

      if (relay == 0)
        relayState = !relayState;

      digitalWrite(relayPin, relayState);
      Blynk.virtualWrite(virtualPin, relayState);
    }
  }

  lastButtonState = buttonState;
}