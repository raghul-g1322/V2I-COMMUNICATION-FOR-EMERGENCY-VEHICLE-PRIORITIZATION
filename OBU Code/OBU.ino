#include <ESP8266WiFi.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// WiFi Configuration
const char* ssid = "Varshhh";        // Your WiFi SSID
const char* password = "Varshhhh"; // Your WiFi password
WiFiServer server(80);  // Create a server that listens on port 80
String direc = "";       // Variable to store the received string

// RF24 Configuration
#define CE_PIN 4   // GPIO4 (D2)
#define CSN_PIN 15 // GPIO15 (D8)
RF24 radio(CE_PIN, CSN_PIN);
String str = ""; // String containing directions (set dynamically)

const byte address[6] = "00001";

void setup() {
  // WiFi Setup
  Serial.begin(115200);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();  // Start the server

  // RF24 Setup
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  // **WiFi String Reception Code**
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {
    Serial.println("New Client.");
    direc = "";  // Clear the previous content of 'direc'

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();  // Read one byte at a time
        Serial.write(c);         // Write the character to the Serial Monitor
        direc += c;              // Append the character to 'direc'
      }
    }
    Serial.println("Complete string received: " + direc); // Print the full received string
    client.stop();

    // Update str with the received string
    str = direc;

    // **OBU RF24 Transmission Code**
    // Loop through each character in the string
    for (int i = 0; i < str.length(); i++) {
      char direction = str[i]; // Get the character at index i

      // Validate the direction
      if (direction == 'L' || direction == 'R' || direction == 'S') {
        radio.write(&direction, sizeof(direction)); // Send the direction
        Serial.println("Direction sent: " + String(direction));
      } else {
        Serial.println("Invalid direction in string: " + String(direction));
      }

      delay(7000); // Wait for 7 seconds before sending the next character
    }

    // After completing the string, stop further execution
    Serial.println("All directions in the string have been sent.");
    while (true); // Halt further execution
  }
}