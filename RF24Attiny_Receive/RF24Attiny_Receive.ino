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
uint8_t pipe;

RF24 radio(CE_PIN, CSN_PIN);  // CE, CSN
SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setup() {
  mySerial.begin(9600);
  mySerial.println("Setup start");

  mySerial.println("Radio.begin");
  radio.begin();
  mySerial.println("Radio.openReadingPipe");
  radio.openReadingPipe(1, address);

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(125);

  radio.startListening();
}

void loop() {
  delay(1000);
  mySerial.println("loop");
  if (radio.isChipConnected()) {
    mySerial.println("Radio chip connected");
    delay(1000);
    if (radio.available(&pipe)) {
      mySerial.println("===========================");
      mySerial.print("Radio availaible, pipe: ");
      mySerial.println(pipe);
      mySerial.println("===========================");

      mySerial.println(radio.getPayloadSize());
        char text[32] = {0};
      radio.read(&text, sizeof(text));
      mySerial.println(text);
    } else {
      mySerial.println("Rien reçu");
    }

  } else {
    mySerial.println("Radio chip not connected");
  }
}
