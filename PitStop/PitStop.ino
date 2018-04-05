//For recieving CAN-BUS data, and storing it to
// the SD-card using LoRa

#include <SoftwareSerial.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <SD.h>

const byte rxPin = 5;
const byte txPin = 6;
const int chipSelect = 4;          // Setting Chipselect to pin 4, this is for the SD-card
//String CANstring = "";            // A string for assembling the data we are logging
char configBuf[4];                // Buffer we are sending to CarTransmitter
const uint8_t add = 0x1A;         // The address in the buffer 00011010
const uint8_t chk = 2;            // The checksum of the buffer
uint8_t loraLen = 4;              // The length of the buffer
const uint8_t CANconfig = 10;     // The config that decides how many CAN messages we want
  


SoftwareSerial uart(rxPin, txPin);
RH_RF95 lora(uart);

void setup() {

  configBuf[0] = add;
  configBuf[1] = chk;
  configBuf[2] = loraLen;
  configBuf[3] = CANconfig;
  
  Serial.begin(115200);

  // SD-card init
  while (!Serial) 
  {
    ; // Wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Initializing SD card");
    
  // See if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized");
  Serial.println("-----------------");
  delay (100);
  
  // LoRa init
  Serial.println("LoRa reciever init ok!");
  while (!lora.init())
  {
    Serial.println("LoRa init failed");
    Serial.println("Init LoRa again");
    delay(100);
  }
  Serial.println("LoRa init ok!");
  Serial.println("-----------------");
  lora.setFrequency(868.0);                      // Setting the frequency to 868 MHz


  Serial.println("Waiting for Setupmessage from CarTransmitter"); // wait for Setupmessage from CarTransmitter
  while(!lora.available())
  {
    Serial.println("Message not available");
    Serial.println("Trying again");
    delay(500);
  }  
   uint8_t loraBufStp[RH_RF95_MAX_MESSAGE_LEN];
   uint8_t recvLen = sizeof(loraBufStp);


   
   if(lora.recv(loraBufStp, &recvLen))       
    {
         if(loraBufStp[0] != add)                        // Checking if the address recieved is the same as address sent 
            {
              Serial.println("Got the wrong address");
              Serial.println("Trying again");
            } 
            else
            {
              Serial.println("-----------------"); 
              Serial.println("Got message: ");
              for(int i = 0; i < recvLen; i++)        // for loop that is printing the buffer we got
              {
                Serial.print("Position ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(loraBufStp[i]);
              }
      
              lora.send(configBuf, loraLen);       // Sending repl to CarTransmitter with the config
              lora.waitPacketSent();
              Serial.println("Sent a reply");
            }
     } 
}

void loop() {
/*
  //waiting for message from CarTransmitter
  Serial.print("Waiting for message from CarTransmitter");
  if(lora.available())
  { 
   uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
   uint8_t recvLen = sizeof(buf);
   lora.recv(buf, &recvLen);
  }
  else 
  { */
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
              for(int i = 0; i < canConfig; i++)
                {
        
                  Serial.print("Position ");
                  Serial.print(i);
                  Serial.print(": ");
                  Serial.print(loraBufStp[i]);
                }
            }
          }
        }
      }
   }
