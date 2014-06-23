/***************************************
LIZARD HABITAT TEMP
by Bithead942

Loops continuously waiting for a network connection.
If a network connection is detected (TCP 8888), check the temp of habitat and retun the value.
   
Uses:
-  Arduino Duemilanove with ATmega328
-  Arduino Ethernet Shield
-  LM34 Farenheit Temperature Sensor  http://www.national.com/mpf/LM/LM34.html#Overview 
-  Magnetic Sensor
*****************************************/
#include <Ethernet.h>
#include <SPI.h>

const int TempPin = 0;    // select the input pin for the Temp Sensor - Analog Port 0
const int LidPin = 2;     // select the input pin for the Magnetic Sensor - Digital Port 2;


byte mac[] = { 0xDE, 0x??, 0xBE, 0xEF, 0x??, 0x?? };      // Replace ?? with Hex
byte ip[] = { 192, 168, ???, ??? };                         // Replace ??? with deciman
int iLidOpen = 0;
boolean bLidOpen = false;

Server server(8888);

void setup() {
  pinMode(TempPin, INPUT);
  pinMode(LidPin, INPUT);
    
  Ethernet.begin(mac, ip);
  server.begin();
  CheckTemp();
}

void loop() {
    Client client = server.available();
    if (client.connected()) {
      server.println(CheckTemp(), DEC);
      if (bLidOpen == true) {
        server.println("1");
      }
      else {
        server.println("0");
      }
      delay(1);
      client.stop();
      bLidOpen = false;
    }
    iLidOpen = digitalRead(LidPin);
    if (iLidOpen == LOW) {
      bLidOpen = true;
    }
}

double sampleToFahrenheit(uint16_t sample) {
  // conversion ratio in DEGREES/STEP:
  // (5000 mV / 1024 steps) * (1 degree / 10mV)
  //	^^^^^^^^^^^		 ^^^^^^^^^^
  //     from ADC		  from LM34
  return sample * (5000.0 / 1024.0 / 10.0);  
}


unsigned int CheckTemp() {
  unsigned int last_sample = 0;
  double this_temp = 0.0;
  double temp_avg = 0.0;
  int i;
  
    for(i=0; i<100; i++) {
      last_sample = analogRead(TempPin);
      this_temp = sampleToFahrenheit(last_sample);

      // add this contribution to the average
      temp_avg = temp_avg + this_temp;
    }
    temp_avg = temp_avg/100.0;
    
    return temp_avg;
}
