/* Include the RFID library */
#include "RFID.h"

/* Define the pins used for the SS (SDA) and RST (reset) pins for BOTH hardware and software SPI */
#define SS_PIN      A2      // Same pin used as hardware SPI (SS)
#define RST_PIN     D2

/* Define the pins used for the DATA OUT (MOSI), DATA IN (MISO) and CLOCK (SCK) pins for SOFTWARE SPI ONLY */
#define MOSI_PIN    A5      // hardware SPI: A5
#define MISO_PIN    A4      //     "     " : A4
#define SCK_PIN     A3      //     "     " : A3

#include "Adafruit_Thermal.h"

/* Create an instance of the RFID library */
#if defined(_USE_SOFT_SPI)
  RFID(int chipSelectPin, int NRSTPD, uint8_t mosiPin, uint8_t misoPin, uint8_t clockPin);    // Software SPI
#else
  RFID RC522(SS_PIN, RST_PIN);    // Hardware SPI
#endif

int printer_RX_Pin = 0;  // Serial1 Rx pin on Spark to the green wire on printer
int printer_TX_Pin = 0;  // Serial1 Tx pin on Spark to the yellow wire on printer

int led = D7; // sets pin

unsigned long lastTime = 0UL;

Adafruit_Thermal printer(printer_RX_Pin, printer_TX_Pin);

// master print command
int print(String args){
  
  int positionEqualsign = 0;
  int positionSlash = 0;
  
  positionEqualsign = args.indexOf("=");
  positionSlash = args.lastIndexOf("/");
  String command = args.substring(0,positionEqualsign);
  String bodyText = args.substring(positionEqualsign+1,positionSlash);
  
  if(command == "BOLD") {
    printBold(bodyText);
    return 1;
  }
  else if(command == "TEXT") {
    printALine(bodyText);
    return 1;
  }
  else if(command == "BIG") {
    printBig(bodyText);
    return 1;
  }
  else if(command == "CENTRE") {
    printCentre(bodyText);
    return 1;
  }
   else if(command == "CENTREBIG") {
    printCentreBig(bodyText);
    return 1;
  }
  else if(command == "CENTREMED") {
    printCentreMedium(bodyText);
    return 1;
  }
  else if(command == "HELLO") {
    printALine(bodyText);
    printer.wake();
    printer.feed(2);
    printer.sleep();
    return 1;
  }
  else {
    return -1;
  }
}


int printImage(String args){
  return 1;
}

int printALine(String args){
   printer.wake();
   printer.println(args);
   printer.sleep();
   return 1;
}

int printBig(String args){
  printer.wake();
  printer.setSize('L');
  printer.println(args);
  printer.setSize('S');
  printer.sleep();
  return 1;
}

int printCentre(String args){
  printer.wake();
  printer.justify('C');
  printer.println(args);
  printer.justify('L');
  printer.sleep();
  return 1;
}

int printBold(String args){
  printer.wake();
  printer.boldOn();
  printer.println(args);
  printer.boldOff();
  printer.sleep();
  return 1;
}

int printCentreBig(String args){
  printer.wake();
  printer.setSize('L');
  printer.justify('C');
  printer.println(args);
  printer.setSize('S');
  printer.justify('L');
  printer.sleep();
  return 1;
}

int printCentreMedium(String args){
  printer.wake();
  printer.setSize('M');
  printer.justify('C');
  printer.println(args);
  printer.setSize('S');
  printer.justify('L');
  printer.sleep();
  return 1;
}



//
//
// program main body
//
//

void setup(){
    
  // initialize pin D7 as an output for use as a blinky LED
  pinMode(led, OUTPUT);
    
  // open serial over USB for debugging
  Serial.begin(9600);
  
  // Enable the SPI interface
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();
  
  // Initialise the RFID reader
  RC522.init();

  // Initialise the printer
  Serial1.begin(19200);
  printer.begin();
  
  Spark.function("print", print);

  printer.feed(3);  
  printer.justify('C'); // centre justify
  printer.println("Welcome to Spark Printer.");
  printer.feed(1);
  printer.println("~");
  printer.feed(5);
  // printer.justify('L'); // left justify
  printer.setDefault(); // Restore printer to defaults
  printer.sleep();      // Tell printer to sleep
}

void loop()
{
  /* Temporary loop counter */
  uint8_t i;
  
  String scannedCardID = "none";
  String cardHex0 = "none";
  String cardHex1 = "none";
  String cardHex2 = "none";
  String cardHex3 = "none";

  unsigned long now = millis();
  //Every 15 seconds publish uptime
  if (now-lastTime>1000UL) {
      lastTime = now;
    
      /* Has a card been detected? */
      if (RC522.isCard())
        {
        /* If so then get its serial number */
        RC522.readCardSerial();
        // tone(A2, 1174.66, 500);
        Serial.println("Card detected:");
      
        /* Output the serial number to the UART */
        
        //   Serial.print(RC522.serNum[0],HEX);
        //   Serial.print(RC522.serNum[1],HEX);
        //   Serial.print(RC522.serNum[2],HEX);
        //   Serial.print(RC522.serNum[3],HEX);
          
          String cardHex0 = String(RC522.serNum[0],HEX);
          String cardHex1 = String(RC522.serNum[1],HEX);
          String cardHex2 = String(RC522.serNum[2],HEX);
          String cardHex3 = String(RC522.serNum[3],HEX);
          
          String scannedCardID = cardHex0 + cardHex1 + cardHex2 + cardHex3;
          
        //   String scannedCardID = "hello";
        //   String cardHex0 = String(RC522.serNum[i],HEX);
        //   Serial.println(cardHex[i]);
        //   String cardhex[i] = String(RC522.serNum[i],HEX);
        
        Serial.println();
        Serial.println("and the card ID is...");
        Serial.println(scannedCardID);
        
        // prints the card ID to the printer for testing purposes
        Spark.publish("scannedCardID",scannedCardID);
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        delay(50);
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        // printer.wake();
        // printer.println("Card detected:");
        // printer.println(scannedCardID);
        // printer.feed(2);
        // printer.sleep();
      }
      else
      {
        Serial.println("Card NOT detected:");
        digitalWrite(led, HIGH);
        delay(50);
        digitalWrite(led, LOW);
        // scannedCardID = "no card";
        // Spark.publish("scannedCardID",scannedCardID);
      }   
    }
}


