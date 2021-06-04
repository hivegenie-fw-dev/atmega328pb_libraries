/* sample for digital weight scale of hx711
 * library design: Weihong Guan (@aguegu)
 * library host on
 *https://github.com/aguegu/ardulibs/tree/3cdb78f3727d9682f7fd22156604fc1e4edd75d1/hx711
 */

//http://arduinotronics.blogspot.de/2015/06/arduino-hx711-digital-scale.html?m=1

// Hx711.DOUT - pin #A2
// Hx711.SCK - pin #A3

#include <hx711.h>
#include <SoftwareSerial.h>
Hx711 scale(A2, A3);

#define Rx  8 // this is physical pin 2
#define Tx  7 // this is physical pin 3

SoftwareSerial mySerial(Rx, Tx);

void setup() {
  mySerial.begin(9600);
}

void loop() {
  mySerial.println(scale.getGram());
  delay(200);
}

