// enables second UART serial on pin D1(TX) and D0(RX)
#include "Serial2/Serial2.h"

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
#include "Adafruit_Thermal/Adafruit_Thermal.h"

/* Create an instance of the RFID library */
#if defined(_USE_SOFT_SPI)
  RFID(int chipSelectPin, int NRSTPD, uint8_t mosiPin, uint8_t misoPin, uint8_t clockPin);    // Software SPI
#else
  RFID RC522(SS_PIN, RST_PIN);    // Hardware SPI
#endif

Adafruit_Thermal printer;

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
   Serial.println(args); // print the line to the USB serial port for debugging
   printer.sleep();
   return 1;
}

int printBold(String args){
  printer.wake();
  printer.boldOn();
  printer.println(args);
  Serial.println(args); // print the line to the USB serial port for debugging
  printer.boldOff();
  printer.sleep();
  return 1;
}


void setup(){
    
  //
  // RFID SETUP
  Serial.begin(9600);
  
  /* Enable the SPI interface */
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();
  
  /* Initialise the RFID reader */
  RC522.init();

  Serial1.begin(19200);
  printer.begin(&Serial1);
  
  Spark.function("print", print);
  Spark.function("printImage", printImage);


  //
  // PRINTER TEST PAGE

  // Test inverse on & off
  printer.inverseOn();
  printer.println("Inverse ON");
  printer.inverseOff();

  // Test character double-height on & off
  printer.doubleHeightOn();
  printer.println("Double Height ON");
  printer.doubleHeightOff();

  // Set text justification (right, center, left) -- accepts 'L', 'C', 'R'
  printer.justify('R');
  printer.println("Right justified");
  printer.justify('C');
  printer.println("Center justified");
  printer.justify('L');
  printer.println("Left justified");

  // Test more styles
  printer.boldOn();
  printer.println("Bold text");
  printer.boldOff();

  printer.underlineOn(); 
  printer.println("Underlined text ");
  printer.underlineOff();

  printer.setSize('L');     // Set type size, accepts 'S', 'M', 'L'
  printer.println("Large"); // Print line
  printer.setSize('M');
  printer.println("Medium");
  printer.setSize('S');
  printer.println("Small");

  printer.justify('C');
  printer.println("normal\nline\nspacing");
  printer.setLineHeight(50);
  printer.println("Taller\nline\nspacing");
  printer.setLineHeight(); // Reset to default
  printer.justify('L');

  printer.println("Adafruit!");
  printer.feed(1);

  printer.sleep();      // Tell printer to sleep
  printer.wake();       // MUST call wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults
}

void loop()
{
  /* Temporary loop counter */
  uint8_t i;

  /* Has a card been detected? */
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();

    Serial.println("Card detected:");

    /* Output the serial number to the UART */
    for(i = 0; i <= 4; i++)
    {
      Serial.print(RC522.serNum[i],HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  else
      Serial.println("Card NOT detected:");
      
  delay(1000);
}
