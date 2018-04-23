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

//File canDataLog;
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
  Serial.println(F("Initializing SD card"));
    
  // See if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) 
  {
    Serial.println(F("Card failed, or not present"));
    while(1);
  }
  Serial.println(F("SD-card initialized"));
  Serial.println(F("-----------------"));
  delay (100);
  
  // LoRa init
  Serial.println(F("LoRa reciever init ok!"));
  while (!lora.init())
  {
    Serial.println(F("LoRa initialized failed"));
    Serial.println(F("Initializing LoRa again"));
    delay(100);
  }
  Serial.println(F("LoRa initialized ok!"));
  Serial.println(F("-----------------"));
  lora.setFrequency(868.0);                      // Setting the frequency to 868 MHz

  Serial.println(F("Waiting for Setupmessage")); // wait for Setupmessage from CarTransmitter
  while(!lora.available())
  {
    Serial.println(F("Message not available"));
    Serial.println(F("Trying again"));
    delay(1000);
  }   
  //loraBufStp[RH_RF95_MAX_MESSAGE_LEN];
  recvLen = sizeof(loraBufStp);
 
  if(lora.recv(loraBufStp, &recvLen))       
    {
         if(loraBufStp[0] != add)                        // Checking if the address recieved is the same as address sent 
            {
              Serial.println(F("Got the wrong address"));
              Serial.println(F("Trying again"));
            } 
            else
            {
              Serial.println(F("-----------------")); 
              Serial.println(F("Got message: "));
              for(int i = 0; i < recvLen; i++)          // for loop that is printing the buffer we got
              {
                Serial.print("Position ");
                Serial.print(i);
                Serial.print(" of the buffer is: ");
                Serial.println(loraBufStp[i], HEX);
              }
              lora.send(configBuf, loraLen);           // Sending repl to CarTransmitter with the config
              lora.waitPacketSent();
              Serial.println(F("Sent a reply"));
            }
     }
}

void loop() {

File canDataLog = SD.open("canData.txt", FILE_WRITE);           // Opens the file
delay(100);
//Recieving CAN-packages
if(lora.available() && canDataLog)
  {
    Serial.println(F("LoRa is available"));
    Serial.println(F("The file is also available"));
   
   // Collecting the recieved data in a buffer and printing it to serial and SD-card
   recvLen = sizeof(canDataBuf);
   if(lora.recv(canDataBuf, &recvLen))                     // Recieving the buffer from CarTransmitter
     {
       Serial.println(F("-----------------")); 
       Serial.println(F("Got CAN messages: "));
       canDataLog.println("-----------------"); 
       canDataLog.println("Got CAN messages: ");
       for(byte i = 0; i < recvLen; i++)                    // Printing the buffer to the serial and SD-Card
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
           
       Serial.println(F("-----------------"));
       canDataLog.println("-----------------");  
       delay(100);         
      } 
    canDataLog.close();
    delay(100);
    }
    else
    {
      //Serial.println("LoRa was not available");
      canDataLog.close();
      delay(100);
    }
  
} 
     

