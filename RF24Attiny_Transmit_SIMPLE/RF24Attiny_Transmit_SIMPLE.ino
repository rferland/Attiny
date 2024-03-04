#include "SoftwareSerial.h"
#include <SPI.h>
#include <RF24.h>

// Define the CE and CSN pins connected to your ATtiny84
#define CE_PIN  2
#define CSN_PIN 3
#define RX_PIN 8  // this is physical pin 2 PB0
#define TX_PIN 9  // this is physical pin 3 PB1

// Create an instance of the RF24 class
RF24 radio(CE_PIN, CSN_PIN);
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// Address configuration
const byte address[6] = "00001";

void setup() {
  // Initialize the SPI interface
  mySerial.begin(9600);
  mySerial.println("Setup start");
  SPI.begin();

  // Initialize the NRF24L01+ module
  radio.begin();
  
  // Set the transmitting radio to use the address
  radio.openWritingPipe(address);
  
  // Set the power level (low, medium, high)
  radio.setPALevel(RF24_PA_LOW);
  
  // Set the data rate
  radio.setDataRate(RF24_250KBPS);
  mySerial.println("Setup end");
}

void loop() {
  // Create a data buffer to hold the data to be sent
  char dataToSend[] = "Hello";

  // Send the data
  radio.write(&dataToSend, sizeof(dataToSend));

  // Delay before sending the next data
  delay(1000);
}
