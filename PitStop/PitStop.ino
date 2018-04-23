//For recieving CAN-BUS data, and storing it to
// the SD-card using LoRa

#include <SoftwareSerial.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <SD.h>

const byte rxPin = 5;
const byte txPin = 6;
const int chipSelect = 4;         // Setting Chipselect to pin 4, this is for the SD-card
char configBuf[4];                // Buffer we are sending to CarTransmitter
const uint8_t add = 0x1A;         // The address in the buffer 00011010
const uint8_t chk = 2;            // The checksum of the buffer
uint8_t loraLen = 4;              // The length of the buffer
const uint8_t CANconfig = 10;     // The config that decides how many CAN messages we want
uint8_t recvLen;
char loraBufStp[4];
char canDataBuf[3 + 10*CANconfig];

File canDataLog;
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
    while(1);
  }
  Serial.println("SD-card initialized");
  Serial.println("-----------------");
  delay (100);
  
  // LoRa init
  Serial.println("LoRa reciever init ok!");
  while (!lora.init())
  {
    Serial.println("LoRa initialized failed");
    Serial.println("Initializing LoRa again");
    delay(100);
  }
  Serial.println("LoRa initialized ok!");
  Serial.println("-----------------");
  lora.setFrequency(868.0);                      // Setting the frequency to 868 MHz

  Serial.println("Waiting for Setupmessage"); // wait for Setupmessage from CarTransmitter
  while(!lora.available())
  {
    Serial.println("Message not available");
    Serial.println("Trying again");
    delay(1000);
  }   
  //loraBufStp[RH_RF95_MAX_MESSAGE_LEN];
  recvLen = sizeof(loraBufStp);
 
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
              for(int i = 0; i < recvLen; i++)          // for loop that is printing the buffer we got
              {
                Serial.print("Position ");
                Serial.print(i);
                Serial.print(" of the buffer is: ");
                Serial.println(loraBufStp[i], HEX);
              }
              lora.send(configBuf, loraLen);           // Sending repl to CarTransmitter with the config
              lora.waitPacketSent();
              Serial.println("Sent a reply");
            }
     }
}

void loop() {

canDataLog = SD.open("canData.txt", FILE_WRITE);    // Opens the file
//Recieving CAN-packages
if(lora.available() && canDataLog)
  {
    Serial.println("LoRa is available");
    Serial.println("The file is also available");
   
   // Collecting the recieved data in a buffer and printing it to serial and SD-card
   recvLen = sizeof(canDataBuf);
   if(lora.recv(canDataBuf, &recvLen))
     {
       Serial.println("-----------------"); 
       Serial.println("Got CAN messages: ");
       canDataLog.println("-----------------"); 
       canDataLog.println("Got CAN messages: ");
       for(int i = 0; i < recvLen; i++)                    // Printing the buffer to the serial to see what CarTransmitter is sending
          {
            Serial.print("Position ");
            Serial.print(i);
            Serial.print(" of the buffer is: ");
            Serial.println(canDataBuf[i], HEX);
              
            canDataLog.print("Position ");
            canDataLog.print(i); 
            canDataLog.print(" of the buffer is: ");         
            canDataLog.println(canDataBuf[i], HEX);
              
           }
       Serial.println("-----------------");
       canDataLog.println("-----------------");           
      } 
   canDataLog.close();
   }
} 
     

