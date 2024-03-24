//#include <SPI.h>
#include <RF24.h>
#include "SoftwareSerial.h"
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <RF24Network.h>
#include <EEPROM.h>

// Define the CE and CSN pins connected to your ATtiny84
#define CE_PIN 2
#define CSN_PIN 3
#define RX_PIN 8  // this is physical pin 2 PB0
#define TX_PIN 9  // this is physical pin 3 PB1

#define START_EEPROM_ADDRESS 0

#define NOT_ASSIGNED 0

uint16_t ClientId = NOT_ASSIGNED;
uint16_t sleepDelay = 9;

// Address configuration
const byte readingAddress[6] = "2Node";
const byte writingAddress[6] = "1Node";

struct __attribute__((__packed__)) {
  uint16_t ID;
  uint16_t Humidity;
  uint16_t Temperature;
  uint16_t Voltage;
} SentDatas;

struct __attribute__((__packed__)) {
  uint16_t ID;
  uint16_t Delay;
  uint16_t Action;
} ReceivedDatas;
// Create an instance of the RF24 class
RF24 radio(CE_PIN, CSN_PIN);
SoftwareSerial mySerial(RX_PIN, TX_PIN);


void setup_watchdog(int ii) {
  // 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9=8 sec

  uint8_t bb;
  if (ii > 9)
    ii = 9;
  bb = ii & 7;
  if (ii > 7)
    bb |= (1 << 5);
  bb |= (1 << WDCE);

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);
}

// system wakes up when watchdog is timed out
void system_sleep(int ii) {
  setup_watchdog(ii);  // approximately 8 seconds sleep

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // sleep mode is set here
  sleep_enable();
  sei();  // Enable the Interrupts so the wdt can wake us up

  sleep_mode();  // System sleeps here

  sleep_disable();  // System continues execution here when watchdog timed out
}
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
  ADMUX = 0b00100001;   // adc source=1.1 ref; adc ref (base for the 1023 maximum)=Vcc
  delay(2);             // Wait for Vref to settle
  ADCSRA |= 1 << ADSC;  // Convert
  while (bit_is_set(ADCSRA, ADSC))
    ;
  result = ADCL;
  result |= ADCH << 8;
  voltage = 1126400L / result;  // Back-calculate AVcc in mV
  return voltage;
}
bool Send() {
  radio.stopListening();  // First, stop listening so we can talk
  delay(10);

  SentDatas.Humidity = 12;
  SentDatas.ID = ReceivedDatas.ID;
  SentDatas.Temperature = 20;
  SentDatas.Voltage = readVcc();

  mySerial.println(F("stopListening"));
  if (radio.write(&SentDatas, sizeof(SentDatas))) {

    mySerial.print(F("Sent response "));

    delay(10);
    return true;
  } else {
    mySerial.println(F("RADIO.WRITE FAILED!"));
  }
}

bool Receive() {
        bool timeOut=false;
  radio.startListening();
  mySerial.println(F("startListening"));
  mySerial.println(F("Radio connected "));
  unsigned long startTime = millis();
  // Check if data is available to be received
  while (!radio.available()){
        // Attend 2 secondes
        if(millis()-startTime >2000){
                timeOut=true;
        break;
        }
  }
  if(timeOut) mySerial.println("Receive time out!");
  if (radio.available()) {
    // Create a buffer to hold the received data
    mySerial.println("Radio availaible");
    // Read the data into the buffer

    radio.read(&ReceivedDatas, sizeof(ReceivedDatas));
    mySerial.println(F(""));
    mySerial.println("[                ]");
    mySerial.println("Radio read OK: ");
    mySerial.print(F("Action: "));
    mySerial.println(ReceivedDatas.Action);
    mySerial.print(F("Delay: "));
    mySerial.println(ReceivedDatas.Delay);
    mySerial.print(F("ID: "));
    mySerial.println(ReceivedDatas.ID);
    mySerial.println("[                ]");
    mySerial.println("");

// Ça ne marche pas ici. On écrase ReceivedDatas
    EEPROM.get(START_EEPROM_ADDRESS, ReceivedDatas);
    if (ClientId != ReceivedDatas.ID || sleepDelay != ReceivedDatas.Delay) {
      ClientId = ReceivedDatas.ID;
      sleepDelay = ReceivedDatas.Delay;
      EEPROM.put(START_EEPROM_ADDRESS,ReceivedDatas);
    }
    return true;
  }
  return false;
}
void setup() {
  mySerial.begin(9600);
  mySerial.println("===========");
  mySerial.println("Setup start");
  mySerial.println("===========");

  // Inclus dans AttinyCore
  SPI.begin();

  // Initialize the NRF24L01+ module
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(76);
  // Set the receiving radio to use the same address
  radio.openReadingPipe(1, readingAddress);
  radio.openWritingPipe(writingAddress);
  radio.startListening();

  // wdt_enable(WDTO_8S);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  mySerial.println("===========");
  mySerial.println("Setup end");
  mySerial.println("===========");
}

void loop() {

  //delay(1000);
  if (!radio.isChipConnected()) {
    // delay(100);
    mySerial.println(F("Radio NOT connected "));

  } else {
    printInfos();
    /* On est connecté*/
    // Start listening for data
    if (Send()) {
      if (Receive()) {
      }
    }
    mySerial.println(F("Go to sleep "));
    delay(100);
    //set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    delay(100);       // Donnez un peu de temps pour que le message soit envoyé avant de dormir.
    system_sleep(sleepDelay);  // Mettez le système en veille en secondes

    mySerial.println(F("I'm back! "));
  }
  // Ajoutez une petite pause pour éviter une utilisation excessive de la CPU et pour stabiliser la boucle.
  delay(10);
}
