// enables second UART serial on pin D1(TX) and D0(RX)
// #include "Serial2/Serial2.h"

/* Include the RFID library */
/* SEE RFID.h for selecting Hardware or Software SPI modes */
#include "RFID.h"

/* Define the pins used for the SS (SDA) and RST (reset) pins for BOTH hardware and software SPI */
/* Change as required */
#define SS_PIN      A2      // Same pin used as hardware SPI (SS)
#define RST_PIN     D2

/* Define the pins used for the DATA OUT (MOSI), DATA IN (MISO) and CLOCK (SCK) pins for SOFTWARE SPI ONLY */
/* Change as required and may be same as hardware SPI as listed in comments */
#define MOSI_PIN    A5      // hardware SPI: A5
#define MISO_PIN    A4      //     "     " : A4
#define SCK_PIN     A3      //     "     " : A3

// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_Thermal.h"

/* Create an instance of the RFID library */
#if defined(_USE_SOFT_SPI)
  RFID(int chipSelectPin, int NRSTPD, uint8_t mosiPin, uint8_t misoPin, uint8_t clockPin);    // Software SPI
#else
  RFID RC522(SS_PIN, RST_PIN);    // Hardware SPI
#endif

int printer_RX_Pin = 0;  // Serial1 Rx pin on Spark to the green wire on printer
int printer_TX_Pin = 0;  // Serial1 Tx pin on Spark to the yellow wire on printer

unsigned long lastTime = 0UL;
char publishString[40];
char scannedCardID[10];

Adafruit_Thermal printer(printer_RX_Pin, printer_TX_Pin);

// master print command
int print(String args){
  // String = "BOLD='Hello'/"
  
  int positionEqualsign = 0;
  int positionSlash = 0;
  
  positionEqualsign = args.indexOf("=");
  positionSlash = args.lastIndexOf("/");
  String command = args.substring(0,positionEqualsign);
  String bodyText = args.substring(positionEqualsign+1,positionSlash);
  
  if(command == "BOLD") {
    printBold(bodyText);
    return 2;
  }
  else if(command == "TEXT") {
    printALine(bodyText);
    return 1;
  }
  else if(command == "HELO") {
    printALine(bodyText);
    // printer.wake();
    // printer.feed(2);
    // printer.sleep();
    return 3;
  }
  else {
    return -1;
  }
}



int printImage(String args){
  return 1;
}

// This command prints a single line, and can called from the Spark API
int printALine(String args){
   printer.wake();
   printer.println(args);
   printer.feed(2);
   printer.sleep();
   return 1;
}

int printBold(String args){
  printer.wake();
  printer.boldOn();
  printer.println(args);
  printer.boldOff();
  printer.feed(2);
  printer.sleep();
  return 1;
}


void setup(){
    
  //
//   RFID SETUP
   Serial.begin(9600);
  
  /* Enable the SPI interface */
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();
  
  /* Initialise the RFID reader */
  RC522.init();

//   Serial.begin(9600);
  
  Serial1.begin(19200);
  printer.begin();
  
  Spark.function("print", print);
//   Spark.function("printImage", printImage);

  printer.println("Team Spark rulez!");
  printer.feed(1);
  printer.boldOn();
  printer.println("Spark Printer is online.");
  printer.boldOff();
  printer.feed(1);
  printer.println("---------------");
  printer.feed(1);
  printer.feed(1);
  printer.feed(1);
  printer.feed(1);

  printer.sleep();      // Tell printer to sleep
  printer.wake();       // MUST call wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults
}

void loop()
{
//   /* Temporary loop counter */
//   uint8_t i;

//   /* Has a card been detected? */
//   if (RC522.isCard())
//   {
//     /* If so then get its serial number */
//     RC522.readCardSerial();
//     // tone(A2, 1174.66, 500);
//     Serial.println("Card detected:");
    
//     /* Output the serial number to the UART */
//     for(i = 0; i <= 4; i++)
//     {
//       Serial.print(RC522.serNum[i],HEX);
//       scannedCardID + (RC522.serNum[i]);
//     }
//     Serial.println();
    
//     Spark.publish("scannedCardID",scannedCardID);
//     scannedCardID == "";
//   }
//   else
//   {
//       Serial.println("Card NOT detected:");
//       scannedCardID == "no card";
//       Spark.publish("scannedCardID",scannedCardID);
//       scannedCardID == "";
//   }   
//   delay(500);

    unsigned long now = millis();
    //Every 15 seconds publish uptime
    if (now-lastTime>15000UL) {
        lastTime = now;
        // now is in milliseconds
        unsigned nowSec = now/1000UL;
        unsigned sec = nowSec%60;
        unsigned min = (nowSec%3600)/60;
        unsigned hours = (nowSec%86400)/3600;
        sprintf(publishString,"%u:%u:%u",hours,min,sec);
        Spark.publish("Uptime",publishString);
    }
}
