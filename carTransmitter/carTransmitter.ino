// For recieving and compiling CAN-BUS signals,
// and sending packets on LoRa

#include <SoftwareSerial.h>
#include <RH_RF95.h>
#include <SPI.h>
#include "mcp_can.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
// Set CS pin. This is for CAN-BUS shield to communicate with Arduino
MCP_CAN CAN(SPI_CS_PIN);    

// Singleton instance of the radio driver
SoftwareSerial ss(5, 6);
RH_RF95 lora(ss);

void setup() {

  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");

  // LoRa init
  Serial.println("LoRa transmitter init");
  if (!lora.init())
  {
    Serial.println("init failed");
    while(1);
  }

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  //lora.setTxPower(13, false);
    
  lora.setFrequency(868.0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
