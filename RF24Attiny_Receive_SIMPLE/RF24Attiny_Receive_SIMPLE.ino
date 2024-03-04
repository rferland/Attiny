#include <SPI.h>
#include <RF24.h>
#include "SoftwareSerial.h"

// Define the CE and CSN pins connected to your ATtiny84
#define CE_PIN 2
#define CSN_PIN 3
#define RX_PIN 8  // this is physical pin 2 PB0
#define TX_PIN 9  // this is physical pin 3 PB1

// Create an instance of the RF24 class
RF24 radio(CE_PIN, CSN_PIN);
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// Address configuration
const byte address[6] = "00001";

void setup() {
  mySerial.begin(9600);
  mySerial.println("Setup start");
  // Initialize the SPI interface
  SPI.begin();

  // Initialize the NRF24L01+ module
  radio.begin();

  // Set the receiving radio to use the same address
  radio.openReadingPipe(1, address);

  // Start listening for data
  radio.startListening();
  mySerial.println("Setup end");
}

void loop() {
  // Check if data is available to be received
  if (radio.available()) {
    // Create a buffer to hold the received data
    char receivedData[32] = { 0 };

    // Read the data into the buffer
    radio.read(&receivedData, sizeof(receivedData));

    // Print the received data
    Serial.println(receivedData);
  }
}
