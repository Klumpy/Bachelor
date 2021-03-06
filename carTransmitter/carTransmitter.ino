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

// instance of the radio drive
const byte rxPin = 5;
const byte txPin = 6;
SoftwareSerial uart(rxPin, txPin);
RH_RF95 lora(uart);

uint8_t add = 0x1a;
uint8_t chk = 1;
uint8_t loraLen;

uint8_t recieveBuf[4];
uint8_t recieveConf;

unsigned long measureStart;
unsigned long measureEnd;
unsigned long wait = 0;

char canBuf[8];
unsigned int canId;
uint8_t canLen = 0;

void setup() {

  char loraBufStp[4];
  loraLen = sizeof(loraBufStp);
  
  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println(" CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println(" CAN BUS Shield init ok!");

  // LoRa init
  Serial.println("LoRa transmitter init:");
  while (!lora.init())
  {
    Serial.println(" LoRa init failed");
    Serial.println(" Init LoRa again");
    delay(100);
  }
  Serial.println(" LoRa init ok!");
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // you can set transmitter powers from 5 to 23 dBm:
  lora.setTxPower(20, false);
    
  lora.setFrequency(868.0);

  loraBufStp[0] = add;
  loraBufStp[1] = chk;
  loraBufStp[2] = loraLen;
  loraBufStp[3] = 7;


  while(recieveBuf[0] != add)
  {
    lora.send(loraBufStp, loraLen);      // send a message to establish connection

    lora.waitPacketSent();            
  
    // wait for konfig
    if(lora.waitAvailableTimeout(5000)) // should be a message ready after 5 sek
    {
      if(lora.recv(recieveBuf, &loraLen))     
      {
        if(recieveBuf[0] == add)
        {
          Serial.println("Connection OK!");
          recieveConf = recieveBuf[3];
        }
        else
        {
          Serial.println("Wrong address!");
        }
      }
      else
      {
        Serial.println("failed to recieve.");
      }
    }
    else
    {
      Serial.println("No response, retransmitting!");
    }
  }
  for(int i = 0; i < loraLen; i++)
  {
    Serial.print("Value of recieve buffer pos ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(recieveBuf[i]);
    Serial.print(", Hex: ");
    Serial.println(recieveBuf[i], HEX);
  }
  
}

void loop() {
// for testing  
  //recieveConf = 10;
  
  char loraBuf[3 + 10*recieveConf];       // set loraBuf to match size of the recieved config
  loraLen = sizeof(loraBuf);

  //Serial.print("Length of lora message: ");
  //Serial.println(loraLen);
  
  loraBuf[0] = add;
  loraBuf[1] = chk;
  loraBuf[2] = loraLen;
  
  int i = 3;
  
  while(i <= loraLen - recieveConf)  // recieveConf for set number of can messages in lora transmit 
  {     
    if(CAN_MSGAVAIL == CAN.checkReceive())
    {
      CAN.readMsgBuf(&canLen, canBuf);
        
      canId = CAN.getCanId();
      
      Serial.print("CAN id: ");
      Serial.println(canId, HEX);

      loraBuf[i] = canId >> 8;              // Print canId to loraBuf
      loraBuf[i + 1] = canId & 0xff;            
         
      for(int j = 0; j < canLen; j++)   // Print canBuf to loraBuf
      { 
        loraBuf[i + j + 2] = canBuf[j]; // +2 bacause first two spots reserved for canId
        Serial.print("buff position: ");
        Serial.print(j + 2);
        Serial.print(" data: ");
        Serial.println(loraBuf[i + j + 2], HEX);
      }
      i = i + recieveConf; 
    }   
    //Serial.print("utenfor if-statement. i = ");
    //Serial.println(i);    
  }
  
  if(wait < 1)
  {
    lora.waitPacketSent();
  }
   
  lora.send(loraBuf, loraLen); 
  measureStart = millis();   
  wait += 1; 
  chk += 1;
  Serial.println(measureStart);
  Serial.println(wait);
  Serial.println("LoRa message sent!");
  Serial.println(F("---------------------------------------"));
  //delay(2000);
  //recieve for time-delay testing
/*    if(lora.waitAvailableTimeout(6000)) // should be a message ready after 5 sek
    {
      if(lora.recv(recieveBuf, &loraLen))     
      {
        measureEnd = millis();
        unsigned long measureDelta = measureEnd - measureStart;
        Serial.print("Time for packet to roundtrip: ");
        Serial.print(measureDelta);
        Serial.println(" ms");
      }
      else
      {
        Serial.println("failed to recieve.");
      }
    }
    else
    {
      Serial.println("No response");
    }
*/
}

