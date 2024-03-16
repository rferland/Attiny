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
const byte readingAddress[6] = "2Node";
const byte writingAddress[6] = "1Node";

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
uint16_t readVcc() {
  uint16_t voltage;
  long result;
  // Read 1.1V reference against AVcc
  ADMUX =  0b00100001;// adc source=1.1 ref; adc ref (base for the 1023 maximum)=Vcc
  delay(2); // Wait for Vref to settle
  ADCSRA |= 1<<ADSC; // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  voltage = 1126400L / result; // Back-calculate AVcc in mV
  return voltage;
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
  radio.openReadingPipe(1, readingAddress);
  radio.openWritingPipe(writingAddress);

  mySerial.println("Setup end");
}

void loop() {
  printInfos();
  if (!radio.isChipConnected()) {
    //delay(100);
    mySerial.println(F("Radio NOT connected "));
  } else {
    /* On est connecté*/
    // Start listening for data
    mySerial.print(F("Voltage: "));
    mySerial.println();
    mySerial.print("");

    radio.startListening();
    mySerial.println(F("startListening"));
    mySerial.println(F("Radio connected "));
    // Check if data is available to be received
    if (radio.available()) {
      // Create a buffer to hold the received data
      unsigned long receivedData;
      mySerial.println("Radio availaible");
      // Read the data into the buffer
      uint16_t data[4] = { 1, 2, 3, 4 };
      radio.read(&data, sizeof(data));
      mySerial.println(F(""));
      mySerial.println("^^^^^^^^^^^^^^^^^^");
      mySerial.println("[                ]");
      mySerial.println("Radio read OK: ");
      mySerial.print(F("data[0]: "));
      mySerial.println(data[0]);
      mySerial.print(F("data[1]: "));
      mySerial.println(data[1]);
      mySerial.print(F("data[2]: "));
      mySerial.println(data[2]);
      mySerial.print(F("data[3]"));
      mySerial.println(data[3]);

      data[0] = 4;
      data[1] = 5;
      data[2] = 6;
      data[3] = readVcc();

      radio.stopListening();  // First, stop listening so we can talk
      delay(10);
      mySerial.println(F("stopListening"));
      if (radio.write(&data, sizeof(data))) {

        mySerial.print(F("Sent response "));
        mySerial.println("[                ]");
        mySerial.println("^^^^^^^^^^^^^^^^^^");
        mySerial.println("");
      }
    }
    delay(1000);
  }
}