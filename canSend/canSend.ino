// testset for send data on CAN-BUS

#include <mcp_can.h>
#include <SPI.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

//sensor IDs
const int hallEffect = 0x01;
const int brakePressure = 0x02;
const int mpu6050 = 0x03;
const int rotaryEncoder = 0x04;

//sensor data
const unsigned char hallDataA[8] = {0, 0, 0, 0, 0, 0, 1, 0xa};
const unsigned char hallDataB[8] = {0, 0, 0, 0, 0, 0, 1, 0xb};
const unsigned char hallDataC[8] = {0, 0, 0, 0, 0, 0, 1, 0xc};
const unsigned char hallDataD[8] = {0, 0, 0, 0, 0, 0, 1, 0xd};

const unsigned char brakeDataA[8] = {0, 0, 0, 0, 2, 0xa, 0, 0};
const unsigned char brakeDataB[8] = {0, 0, 0, 0, 2, 0xb, 0, 0};

const unsigned char mpuDataA[8] = {0, 0, 3, 0xa, 0, 0, 0, 0};
const unsigned char mpuDataB[8] = {0, 0, 3, 0xb, 0, 0, 0, 0};
const unsigned char mpuDataC[8] = {0, 0, 3, 0xc, 0, 0, 0, 0};
const unsigned char mpuDataD[8] = {0, 0, 3, 0xd, 0, 0, 0, 0};

const unsigned char rotaryDataA[8] = {4, 0xa, 0, 0, 0, 0, 0, 0};
const unsigned char rotaryDataB[8] = {4, 0xb, 0, 0, 0, 0, 0, 0};

void setup() {

    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
}

void loop() {

  //send data
  CAN.sendMsgBuf(hallEffect, 0, 8, hallDataA);
  delay(100);
  CAN.sendMsgBuf(hallEffect, 0, 8, hallDataB);
  delay(100);                                    
  CAN.sendMsgBuf(hallEffect, 0, 8, hallDataC);
  delay(100);
  CAN.sendMsgBuf(hallEffect, 0, 8, hallDataD);
  delay(100);
  CAN.sendMsgBuf(brakePressure, 0, 8, brakeDataA);
  delay(100);
  CAN.sendMsgBuf(brakePressure, 0, 8, brakeDataB);
  delay(100);
  CAN.sendMsgBuf(mpu6050, 0, 8, mpuDataA);
  delay(100);
  CAN.sendMsgBuf(mpu6050, 0, 8, mpuDataB);
  delay(100);
  CAN.sendMsgBuf(mpu6050, 0, 8, mpuDataC);
  delay(100);
  CAN.sendMsgBuf(mpu6050, 0, 8, mpuDataD);
  delay(100);
  CAN.sendMsgBuf(rotaryEncoder, 0, 8, rotaryDataA);
  delay(100);           
  CAN.sendMsgBuf(rotaryEncoder, 0, 8, rotaryDataB);
  delay(100);                                       //total send time 1.2s

}

