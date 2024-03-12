
/*
 * Getting Started example sketch for nRF24L01+ radios
 * This is a very basic example of how to send data from one node to another
 * Updated: Dec 2014 by TMRh20
 */

// #include <SPI.h>
#include "RF24.h"
#include "SoftwareSerial.h"

#define TRANSMIT true
#define RECEIVE false
/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(2, 3);
/**********************************************************/

byte addresses[][6] = { "1Node", "2Node" };

// Used to control whether this node is sending or receiving
bool role = RECEIVE;

#define RX_PIN 8  // this is physical pin 2 PB0
#define TX_PIN 9  // this is physical pin 3 PB1
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// Leds
#define LED_RED 0
#define LED_YELLOW 1
#define LED_GREEN 7

void ledOff(uint8_t ledPin) {
  digitalWrite(ledPin, LOW);
}

void blinkLed(uint8_t ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(10);
  digitalWrite(ledPin, LOW);
}
void printInfos() {
        mySerial.println("");
  mySerial.println("/-************************************-");
  uint8_t channelNo = radio.getChannel();
  mySerial.print("DataRate: ");
  mySerial.println(radio.getDataRate());
  mySerial.print("Channel: ");
  mySerial.println(channelNo);
  mySerial.print("DynamicPayloadSize: ");
  mySerial.println(radio.getDataRate());
  mySerial.print("PALevel: ");
  mySerial.println(radio.getPALevel());
  mySerial.println("/-************************************-");
  mySerial.println("");
}
void setup() {

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  mySerial.begin(9600);
  mySerial.println(F("RF24/examples/GettingStarted"));
  mySerial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  // radio.setChannel(76);
  // Open a writing and reading pipe on each radio, with opposite addresses
  if (radioNumber) {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
  } else {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }

  // Start the radio listening for data
  radio.startListening();
  mySerial.println(F("Set up terminated"));
  ledOff(LED_GREEN);
  // ledOff(LED_YELLOW);
  ledOff(LED_RED);
}

void loop() {

  

  // blinkLed(LED_RED);
  if (!radio.isChipConnected()) {
    delay(1000);
    mySerial.println(F("Radio not connected "));
    if (role == TRANSMIT) {
      mySerial.println(F("Role=Transmit"));
    } else {
      mySerial.println(F("Role=RECEIVE"));
      ;
    }
  } else {
    mySerial.println(F("++++++++++++++++++++++++++++++++++ "));
    mySerial.println(F("Radio connected "));
    mySerial.print(F("Role =  "));
    mySerial.println(role);
    mySerial.println(F("++++++++++++++++++++++++++++++++++ "));

    /****************** Ping Out Role ***************************/
//     if (role) {

//       mySerial.println(F("radio.stopListening "));
//       radio.stopListening();  // First, stop listening so we can talk.
//       delay(100);

//       mySerial.println(F("Now sending"));

//       unsigned long start_time = micros();  // Take the time, and send it.  This will block until complete
//       if (!radio.write(&start_time, sizeof(unsigned long))) {
//         mySerial.println(F("failed"));
//       } else {
//         mySerial.print(F("sent "));
//         mySerial.print(start_time);
//       }

//       radio.startListening();  // Now, continue listening

//       unsigned long started_waiting_at = micros();  // Set up a timeout period, get the current microseconds
//       boolean timeout = false;                      // Set up a variable to indicate if a response was received or not

//       while (!radio.available()) {                     // While nothing is received
//         if (micros() - started_waiting_at > 200000) {  // If waited longer than 200ms, indicate timeout and exit while loop
//           timeout = true;
//           break;
//         }
//       }

//       if (timeout) {  // Describe the results
//         mySerial.println(F("Failed, response timed out."));
//       } else {
//         unsigned long got_time;  // Grab the response, compare, and send to debugging spew
//         radio.read(&got_time, sizeof(unsigned long));
//         unsigned long end_time = micros();

//         // Spew it
//         mySerial.print(F("Sent "));
//         mySerial.print(start_time);
//         mySerial.print(F(", Got response "));
//         mySerial.print(got_time);
//         mySerial.print(F(", Round-trip delay "));
//         mySerial.print(end_time - start_time);
//         mySerial.println(F(" microseconds"));
//       }
//     }
    // Try again 1s later
    delay(1000);

    /****************** Pong Back Role ***************************/

    if (!role) { // Receive
      mySerial.println(F("role == RECEIVE"));
      unsigned long got_time;  // Variable for the received timestamp
      printInfos();
      if (radio.available()) {
        while (radio.available()) {                      // While there is data ready
          radio.read(&got_time, sizeof(unsigned long));  // Get the payload
          mySerial.println(F("radio.available"));
          mySerial.print(F("radio.read() got_time: "));
          mySerial.println(got_time);
        }
        mySerial.println(F("radio.stopListening"));
        radio.stopListening();
        bool writeOK = radio.write(&got_time, sizeof(unsigned long));  // First, stop listening so we can talk
        if (writeOK) {
          mySerial.print(F("Sent response "));
          mySerial.println(got_time);
        }
        mySerial.println(F("radio.startListening"));
        // Send the final one back.
        radio.startListening();  // Now, resume listening so we catch the next packets.
      } else {
        mySerial.println(F("radio NOT available"));
      }
    }

    /****************** Change Roles via mySerial Commands ***************************/
    if (mySerial.available()) {
      char c = toupper(mySerial.read());
      if (c == 'T' && role == RECEIVE) {
        mySerial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
        role = TRANSMIT;  // Become the primary transmitter (ping out)
      } else if (c == 'R' && role == TRANSMIT) {
        mySerial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
        role = RECEIVE;  // Become the primary receiver (pong back)
        radio.startListening();
      }
    }
  }

  delay(1000);

}  // Loop
