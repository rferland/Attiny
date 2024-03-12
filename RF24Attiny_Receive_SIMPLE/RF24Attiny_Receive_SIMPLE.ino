//#include <SPI.h>
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
const byte address[6] = "2Node";

void printInfos() {
  mySerial.println("");
  mySerial.println("/-************************************-");
  mySerial.print("DataRate: ");
  mySerial.println(radio.getDataRate());
  mySerial.print("Channel: ");
  mySerial.println(radio.getChannel());
  mySerial.print("DynamicPayloadSize: ");
  mySerial.println(radio.getDataRate());
  mySerial.print("PALevel: ");
  mySerial.println(radio.getPALevel());
  mySerial.println("/-************************************-");
  mySerial.println("");
}

void setup() {
  mySerial.begin(9600);
  mySerial.println("===========");
  mySerial.println("Setup start");
  mySerial.println("===========");
  // Initialize the SPI interface
  // SPI.begin();

  // Initialize the NRF24L01+ module
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(76);
  //Set the receiving radio to use the same address
  radio.openReadingPipe(1, address);

  // Start listening for data
  radio.startListening();
  mySerial.println("Setup end");
}

void loop() {
  printInfos();
  if (!radio.isChipConnected()) {
    //delay(100);
    mySerial.println(F("Radio NOT connected "));
  } else {
    mySerial.println(F("Radio connected "));
    // Check if data is available to be received
    if (radio.available()) {
      // Create a buffer to hold the received data
      unsigned long receivedData;
      mySerial.println("Radio availaible");
      // Read the data into the buffer
      radio.read(&receivedData, sizeof(unsigned long));
      mySerial.println("");
      mySerial.println("^^^^^^^^^^^^^^^^^^");
      mySerial.println("[                ]");
      mySerial.print("Radio read OK: ");
      mySerial.println(receivedData);
      mySerial.println("[                ]");
      mySerial.println("^^^^^^^^^^^^^^^^^^");
      mySerial.println("");
      
    }
  }
  delay(1000);
}
