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
uint8_t add = 0x1A;         //The address in the buffer 00011010
uint8_t chk = 2;            //The checksum of the buffer
uint8_t loraLen = 4;        //The length of the buffer
uint8_t CANconfig = 10;     //The config that decides how many CAN messages we want
  


SoftwareSerial uart(rxPin, txPin);
RH_RF95 lora(uart);

void setup() {

  configBuff[0] = add;
  configBuff[1] = chk;
  configBuff[2] = loraLen;
  configBuff[3] = CANconfig;
  
  Serial.begin(115200);

  //SD-card init
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Initializing SD card");
    
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized");
  Serial.println("-----------------");
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
  Serial.println("-----------------");
  lora.setFrequency(868.0);

//wait for Setupmessage from CarTransmitter
  Serial.println("Waiting for Setupmessage from CarTransmitter"); 
  while(!lora.available())
  {
    Serial.println("Message not available");
    Serial.println("Trying again");
    delay(300);
  }  
   uint8_t loraBufStp[RH_RF95_MAX_MESSAGE_LEN];
   uint8_t recvLen = sizeof(loraBufStp);
   if(lora.recv(loraBufStp, &recvLen))
    {
      Serial.println("-----------------"); 
      Serial.println("Got request: ");
      Serial.println(loraBufStp[0],HEX);
      Serial.println(loraBufStp[1]); 
      Serial.println(loraBufStp[2]); 
      Serial.println(loraBufStp[3]); 
      Serial.println("-----------------"); 
     
      //Sender svar til CarTransmitter med config
      lora.send(configBuff, loraLen);
      lora.waitPacketSent();
      Serial.println("Sent a reply");
    }
}

void loop() {

//waiting for message from CarTransmitter
Serial.print("Waiting for message from CarTransmitter");
if(lora.available())
{ 
   uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
   uint8_t recvLen = sizeof(buf);
   lora.recv(buf, &recvLen);
   
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
        uint8_t recvLen = sizeof(buf);
        lora.recv(buf, &recvLen);
        if(lora.recv(buf, &recvLen))
            {
              Serial.println("-----------------"); 
              Serial.println("Got request: ");
              Serial.println(loraBufStp[0],HEX);
              Serial.println(loraBufStp[1]); 
              Serial.println(loraBufStp[2]); 
              Serial.println(loraBufStp[3]); 
              Serial.println("-----------------");
            }
          }
        }
      } 
   }
