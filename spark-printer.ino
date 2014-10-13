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
#define MOSI_PIN    D3      // hardware SPI: A5
#define MISO_PIN    D4      //     "     " : A4
#define SCK_PIN     D5      //     "     " : A3

// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_Thermal/Adafruit_Thermal.h"

Adafruit_Thermal printer;

// This command prints a single line, and can called from the Spark API
int printLine(String args){
   printer.wake();
   printer.println(args);
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

int print(String args){
  // String = "BOLD='Hello'/"
  
  int positionEqualsign = 0;
  int positionSlash = 0;
  
  positionEqualsign = args.indexOf("=");
  positionSlash = args.lastIndexOf("/");
  String command = args.substring(0,positionEqualsign);
  String bodyText = args.substring(positionEqualsign,positionSlash);
  
  if(command == "BOLD")
    printBold(bodyText);
  else if(command == "TEXT")
    printLine(bodyText);
  else
    return -1;
}

void setup(){
  Serial1.begin(19200);
  printer.begin(&Serial1);
  
  Spark.function("printLine", printLine);
  Spark.function("printBold", printBold);

  // The following function calls are in setup(), but do not need to be.
  // Use them anywhere!  They're just here so they're run only one time
  // and not printed over and over.
  // Some functions will feed a line when called to 'solidify' setting.
  // This is normal.

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

}
