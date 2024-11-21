#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 4   // GPIO4 (D2)
#define CSN_PIN 15 // GPIO15 (D8)

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

#define LED_RED 16         // GPIO2 (D4) - Red LED
#define LED_LEFT 5    // GPIO5 (D1)
#define LED_STRAIGHT 2 // GPIO2 (D4)
#define LED_RIGHT 0   // GPIO0 (D3)


void setup() {
  Serial.begin(9600);

  // Configure LED pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_STRAIGHT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);

  // Set the default state
  digitalWrite(LED_RED, HIGH); // Red LED ON by default
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_STRAIGHT, LOW);
  digitalWrite(LED_RIGHT, LOW);

  // Initialize RF24 module
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  Serial.println("RSU is ready to receive data...");
}

void loop() {
  if (radio.available()) {
    char direction;
    radio.read(&direction, sizeof(direction));
    Serial.println("Direction received: " + String(direction));

    // Turn OFF red LED and reset all green LEDs
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_LEFT, LOW);
    digitalWrite(LED_STRAIGHT, LOW);
    digitalWrite(LED_RIGHT, LOW);

    // Light up the appropriate green LED
    if (direction == 'L') {
      digitalWrite(LED_LEFT, HIGH);
    } else if (direction == 'R') {
      digitalWrite(LED_RIGHT, HIGH);
    } else if (direction == 'S') {
      digitalWrite(LED_STRAIGHT, HIGH);
    }

    // Keep green LED ON for 5 seconds
    delay(3000);

    // Turn OFF all green LEDs and turn ON red LED
    digitalWrite(LED_LEFT, LOW);
    digitalWrite(LED_STRAIGHT, LOW);
    digitalWrite(LED_RIGHT, LOW);
    digitalWrite(LED_RED, HIGH);
  }
}