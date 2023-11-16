 // SerialDataLink.h
 
 /**
 * @file SerialDataLink.h
 * @brief Header file for the SerialDataLink library, used for reliable serial communication.
 *
 * This library provides functionalities for serial communication between Arduino devices,
 * including packet construction, data transmission and reception, CRC checking, and error handling.
 */


#ifndef SERIALDATALINK_H
#define SERIALDATALINK_H

#include <Arduino.h>
#include <Streaming.h>
#include <PString.h>

/**
 * @class SerialDataLink
 * @brief Class providing serial communication capabilities.
 *
 * SerialDataLink is a class that facilitates serial communication between Arduino devices.
 * It includes features like CRC checking for data integrity, packet construction, and error handling.
 */

class SerialDataLink
{
public:

     /**
     * @brief Construct a new Serial Data Link object.
     * 
     * @param serial Reference to the serial stream.
     * @param address Device address in the network.
     * @param maxIndex Maximum index for data array.
     * @param enableRetransmit Flag to enable or disable automatic retransmission.
     */
  SerialDataLink(Stream &serial, uint8_t address, uint8_t maxIndex,  bool enableRetransmit = false);
     /**
     * @brief Main method to be called in the loop to handle data transmission.
     */
  void run();
     /**
     * @brief Updates the data at a specified index.
     * 
     * @param index Index of the data to be updated.
     * @param value New value to update.
     */
  void updateData(uint8_t index, int16_t value);
  bool checkTransmissionError(bool resetFlag);
  void setHeaderChar(char header);
  void setEOTChar(char eot);
  void setACKChar(char ack);
     /**
     * @brief Main method to be called in the loop to handle data reception.
     */
  void read();
  int16_t readData(uint8_t index);
  bool checkNewData(bool resetFlag);
  bool checkReadError(bool reset);

  void setUpdateInterval(unsigned long interval);
  void setAckTimeout(unsigned long timeout);
  void setPacketTimeout(unsigned long timeout);
  

private:
  static const uint8_t bufferSize = 128;  
  uint8_t buffer[bufferSize];
  uint8_t bufferIndex;
  uint8_t txBufferIndex=0;
  Stream &serial;

  static const uint8_t dataArraySize = 20;  
  unsigned long lastUpdateTime[dataArraySize];
  int16_t dataArray[dataArraySize];
  
  bool dataUpdated[dataArraySize];
  unsigned long updateInterval = 500; 
  unsigned long lastTransmissionTime;
  unsigned long ACK_TIMEOUT = 100; 

  const uint8_t maxIndex;
  uint8_t intendedAddress;
  bool isTransmitting;
  bool retransmitEnabled;
  bool transmissionError = false;

  char headerChar = '<';  
  char eotChar = '>';     
  char ackChar = 0x06;    

  static const uint16_t RESEND_ALL_COMMAND = 0xFEED;

  void handleTransmissionTimeout();
  void constructPacket();
  bool sendNextByte();
  void addToBuffer(uint8_t byte);
  void checkForCommands();
  void handleResendRequest();
  void receivedACK();

  bool isCompletePacket();
  void processPacket();
  void sendACK();
  
  bool checkCRC();
  
  uint16_t calculateCRC16(const uint8_t* data, size_t length);

  static const uint8_t receiveBufferSize = 128;
  uint8_t receiveBuffer[receiveBufferSize];
  uint8_t receiveBufferIndex = 0;
  unsigned long lastHeaderTime = 0;
  unsigned long PACKET_TIMEOUT = 150; // Timeout in milliseconds
  bool readError = false;
  bool newData = false;
};

#endif
