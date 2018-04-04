//For recieving CAN-BUS data, and storing it to
// the SD-card using LoRa

#include <SoftwareSerial.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <SD.h>

const byte rxPin = 5;
const byte txPin = 6;
const int chipSelect = 4;   //Setting Chipselect to pin 4, this is for the SD-card
String CANstring = "";      //A string for assembling the data we are logging
char LoraSendBuff[4];       //Buffer we are sending to CarTransmitter
uint8_t add = 1;       //The address in the buffer
uint8_t chk = 2;       //The checksum of the buffer
uint8_t len = 4;         //The length of the buffer
uint8_t CANconfig = 10;     //The config that decides how many CAN messages we want
  
LoraSendBuff[0] = add;
LoraSendBuff[1] = chk;
LoraSendBuff[2] = len;
LoraSendBuff[3] = CANconfig;

SoftwareSerial uart(rxPin, txPin);
RH_RF95 lora(uart);

void setup() {

  Serial.begin(115200);

  //SD-card init
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card");
    
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized");
  Serial.print("-----------------------------");
  delay (100);
  
  //LoRa init
  Serial.println("LoRa reciever init ok!");
  while (!lora.init())
  {
    Serial.println("LoRa init failed");
    Serial.println("Init LoRa again");
    delay(100);
  }
  Serial.println("LoRa init ok!");
  lora.setFrequency(868.0);
  Serial.print("-----------------------------");

//wait for message from CarTransmitter
Serial.print("Waiting for message from CarTransmitter"); 
if(lora.available())
  {  
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if(lora.recv(buf, &len))
    {
     Serial.print("got request: ");
     Serial.println((char*)buf); 
     
     //Sender svar til CarTransmitter med config
      lora.send(LoraSendBuff, len);
      lora.waitPacketSent();
      Serial.println("Sent a reply");
    } 
  }
}

void loop() {

//waiting for message from CarTransmitter
Serial.print("Waiting for message from CarTransmitter");
if(lora.available())
{ 
   uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
   uint8_t len = sizeof(buf);
   lora.recv(buf, &len);
   
  if (buf[0] != add)   //Checking if the address recieved is the same as address sent 
  {
    SMS.flush();
    Serial.print("Got the wrong address");
    Serial.print("Trying again");
    continue;   
  }  
  else 
  {
      //Recieving CAN-packages
      if(lora.available())
      {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        lora.recv(buf, &len);
        if(lora.recv(buf, &len))
            {
              Serial.print("Got Payload: ");
              Serial.println((char*)buf);
            }
          }
        }
      } 
   }
