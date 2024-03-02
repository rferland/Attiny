#include <RF24.h>
#include "SoftwareSerial.h"
/*
Configuration Parameters: Double-check the configuration parameters for the nRF24L01+, 
especially the channel, data rate, and power settings, 
to ensure they match on both the transmitting and receiving ends.

ON DOIT METTRE LE SERIAL MONITOR À 1200 BAUDS
*/
const byte address[6] = "00001";
#define RX_PIN 8  // this is physical pin 2 PB0
#define TX_PIN 9  // this is physical pin 3 PB1
#define CE_PIN 2
#define CSN_PIN 3

RF24 radio(CE_PIN, CSN_PIN);  // CE, CSN
SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setup() {
  mySerial.println("Setup start");
  mySerial.begin(9600);
  radio.setAutoAck(true);
  mySerial.println("Radio.begin");
  radio.begin();
  mySerial.println("Radio.openWritingPipe");
  radio.openWritingPipe(address);
  radio.setRetries(15, 15);

  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(125); 


  mySerial.println("Radio.stopListening");
  radio.stopListening();
}

void loop() {
  mySerial.println("loop");
  if (radio.isChipConnected()) {
    mySerial.println("Radio.isChipConnected OK");
    const char text[] = "Hello World";
    bool result = radio.write(&text, sizeof(text));
    if (result) {
      mySerial.println("Radio.write OK");
    } else {
      mySerial.println("Radio.write ERROR");
    }
  } else {
    mySerial.println("Radio.isChipConnected ERREUR");
  }

  delay(1000);
}
