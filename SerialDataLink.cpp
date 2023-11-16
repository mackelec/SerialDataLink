// SerialDataLink.cpp

#include "SerialDataLink.h"


const uint16_t crcTable[256] = {
    0, 32773, 32783, 10, 32795, 30, 20, 32785,
    32819, 54, 60, 32825, 40, 32813, 32807, 34,
    32867, 102, 108, 32873, 120, 32893, 32887, 114,
    80, 32853, 32863, 90, 32843, 78, 68, 32833,
    32963, 198, 204, 32969, 216, 32989, 32983, 210,
    240, 33013, 33023, 250, 33003, 238, 228, 32993,
    160, 32933, 32943, 170, 32955, 190, 180, 32945,
    32915, 150, 156, 32921, 136, 32909, 32903, 130,
    33155, 390, 396, 33161, 408, 33181, 33175, 402,
    432, 33205, 33215, 442, 33195, 430, 420, 33185,
    480, 33253, 33263, 490, 33275, 510, 500, 33265,
    33235, 470, 476, 33241, 456, 33229, 33223, 450,
    320, 33093, 33103, 330, 33115, 350, 340, 33105,
    33139, 374, 380, 33145, 360, 33133, 33127, 354,
    33059, 294, 300, 33065, 312, 33085, 33079, 306,
    272, 33045, 33055, 282, 33035, 270, 260, 33025,
    33539, 774, 780, 33545, 792, 33565, 33559, 786,
    816, 33589, 33599, 826, 33579, 814, 804, 33569,
    864, 33637, 33647, 874, 33659, 894, 884, 33649,
    33619, 854, 860, 33625, 840, 33613, 33607, 834,
    960, 33733, 33743, 970, 33755, 990, 980, 33745,
    33779, 1014, 1020, 33785, 1000, 33773, 33767, 994,
    33699, 934, 940, 33705, 952, 33725, 33719, 946,
    912, 33685, 33695, 922, 33675, 910, 900, 33665,
    640, 33413, 33423, 650, 33435, 670, 660, 33425,
    33459, 694, 700, 33465, 680, 33453, 33447, 674,
    33507, 742, 748, 33513, 760, 33533, 33527, 754,
    720, 33493, 33503, 730, 33483, 718, 708, 33473,
    33347, 582, 588, 33353, 600, 33373, 33367, 594,
    624, 33397, 33407, 634, 33387, 622, 612, 33377,
    544, 33317, 33327, 554, 33339, 574, 564, 33329,
    33299, 534, 540, 33305, 520, 33293, 33287, 514
};

union Convert
{
  uint16_t u16;
  int16_t i16;
  struct
  {
    byte low;
    byte high;
  };
}convert;

SerialDataLink::SerialDataLink(Stream &serial,uint8_t address, uint8_t maxIndex,  bool enableRetransmit)
  : serial(serial), maxIndex(maxIndex), intendedAddress(address), retransmitEnabled(enableRetransmit)
{
  memset(dataArray, 0, sizeof(dataArray));
  memset(dataUpdated, 0, sizeof(dataUpdated));
  memset(lastUpdateTime,0,sizeof(lastUpdateTime));
  memset(dataUpdated,0,sizeof(dataUpdated));
}

void SerialDataLink::updateData(uint8_t index, int16_t value)
{
  if (index < maxIndex)
  {
    if (dataArray[index] != value)
    {
      dataArray[index] = value;
      dataUpdated[index] = true;
      lastUpdateTime[index] = millis();
    }
  }
}




void SerialDataLink::run()
{
  // Handle sending the next byte if currently transmitting
  if (isTransmitting)
  {
    sendNextByte();
  }
  // Check for conditions to construct and send a new packet
  else 
  {
    constructPacket();
  }

  // Check for ACK timeouts
  if (isTransmitting && (millis() - lastTransmissionTime > ACK_TIMEOUT))
  {
    handleTransmissionTimeout();
  }

  // Check for incoming commands or ACKs
  checkForCommands();
}

bool SerialDataLink::checkTransmissionError(bool resetFlag) 
{
  bool currentStatus = transmissionError;
  if (resetFlag && transmissionError) {
    transmissionError = false;
  }
  return currentStatus;
}

void SerialDataLink::setUpdateInterval(unsigned long interval) {
    updateInterval = interval;
}

void SerialDataLink::setAckTimeout(unsigned long timeout) {
    ACK_TIMEOUT = timeout; // Make sure ACK_TIMEOUT is not declared as const
}

void SerialDataLink::setPacketTimeout(unsigned long timeout) {
    PACKET_TIMEOUT = timeout; // Make sure PACKET_TIMEOUT is not declared as const
}


void SerialDataLink::constructPacket()
{
  if (!isTransmitting)
  {
    lastTransmissionTime = millis();
    bufferIndex = 0;
    addToBuffer(headerChar);
    addToBuffer(intendedAddress);
    unsigned long currentTime = millis();
    int count = bufferIndex;
    for (uint8_t i = 0; i < maxIndex; i++)
    {
      if (dataUpdated[i] || (currentTime - lastUpdateTime[i] >= updateInterval))
      {
        addToBuffer(i);
        convert.i16 = dataArray[i];
        addToBuffer(convert.high);
        addToBuffer(convert.low);
        dataUpdated[i] = false;
        lastUpdateTime[i] = currentTime;
      }
    }
    if (count == bufferIndex)
    {
      bufferIndex = 0;
      return;
    }
    addToBuffer(eotChar);
    uint16_t crc = calculateCRC16(buffer, bufferIndex);
    convert.u16 = crc;
    addToBuffer(convert.high);
    addToBuffer(convert.low);

    isTransmitting = true;
    
  }
}

bool SerialDataLink::sendNextByte()
{
  if (!isTransmitting) return false;
  if (bufferIndex >= bufferSize || txBufferIndex >= bufferSize)
  {
    bufferIndex = 0;
    isTransmitting = false;
    return false;
  }
  
  txBufferIndex ++;
  if (txBufferIndex => bufferIndex)
  {
    isTransmitting = false;
    bufferIndex = 0;
    txBufferIndex = 0;
    Serial << "Packet sent" << endl;
    return true; // Packet sent
  }
 
  return false; // Packet not yet fully sent
}

void SerialDataLink::receivedACK()
{
  isTransmitting = false;
  // Additional logic for post-ACK processing if needed
}



void SerialDataLink::setHeaderChar(char header)
{
  headerChar = header;
}

void SerialDataLink::setEOTChar(char eot)
{
  eotChar = eot;
}

void SerialDataLink::setACKChar(char ack)
{
  ackChar = ack;
}

void SerialDataLink::addToBuffer(uint8_t byte)
{
  if (bufferIndex < bufferSize)
  {
    buffer[bufferIndex] = byte;
    bufferIndex++;
  }
}


void SerialDataLink::checkForCommands()
{
  while (serial.available() > 1) // Checking for at least 2 bytes (size of 0xFEED)
  {
    // Peek at the next byte without removing it from the serial buffer
    char nextChar = serial.peek();

    // Check for ACK character
    if (nextChar == ackChar)
    {
      serial.read(); // Remove the ACK character from the buffer
      receivedACK();
    }
    // Check for 0xFEED command
    else
    {
      uint16_t command = 0;
      command |= (serial.read() << 8); // Read the first byte
      if (serial.available() > 0) 
      {
        command |= serial.read(); // Read the second byte

        if (command == RESEND_ALL_COMMAND)
        {
          handleResendRequest();
        }
      }
    }
  }
}

void SerialDataLink::handleTransmissionTimeout() 
{
  // Logic for handling transmission timeout
  // For example: Retry transmission, set error flag, etc.
  transmissionError = true;
  isTransmitting = false;
}

void SerialDataLink::handleResendRequest()
{
  for (uint8_t i = 0; i < dataArraySize; ++i)
  {
    dataUpdated[i] = true;
  }
}

bool SerialDataLink::checkNewData(bool resetFlag) {
  bool currentStatus = newData;
  if (resetFlag && newData) {
    newData = false;
  }
  return currentStatus;
}

int16_t SerialDataLink::readData(uint8_t index) {
  if (index < dataArraySize) {
    return dataArray[index];
  } else {
    // Handle the case where the index is out of bounds
    // You could return a special error value or handle it in another way
    return -1; // For example, returning -1 as an error indication
  }
}

void SerialDataLink::read()
{
  while (serial.available()) 
  {
    //Serial.print(".");
    if (millis() - lastHeaderTime > PACKET_TIMEOUT  && receiveBufferIndex > 0) 
    {
      // Timeout occurred, reset buffer and pointer
      receiveBufferIndex = 0;
      readError = true;
    }
    uint8_t incomingByte = serial.read();
    switch (receiveBufferIndex) {
      case 0: // Looking for the header
        if (incomingByte == headerChar) 
        {
          lastHeaderTime = millis();
          receiveBuffer[receiveBufferIndex] = incomingByte;
          receiveBufferIndex++;
        }
        break;

      case 1: // Looking for the address
        if (incomingByte == intendedAddress) {
          receiveBuffer[receiveBufferIndex] = incomingByte;
          receiveBufferIndex++;
        } else {
          // Address mismatch, reset to look for a new packet
          receiveBufferIndex = 0;
        }
        break;

      default:
        // Normal data handling
        receiveBuffer[receiveBufferIndex] = incomingByte;
        receiveBufferIndex++;
        
        if (isCompletePacket()) {
          processPacket();
          receiveBufferIndex = 0; // Reset for the next packet
        }

        // Check for buffer overflow
        if (receiveBufferIndex >= receiveBufferSize) {
          // Handle overflow
          receiveBufferIndex = 0;
        }
        break;
    }
  }
}



bool SerialDataLink::isCompletePacket() {
  // Ensure there are enough bytes for EOT + 2-byte CRC
  if (receiveBufferIndex >= 3) {
    // Check if the third-last byte is the EOT character
    if (receiveBuffer[receiveBufferIndex - 3] == eotChar) {
      // The last three bytes are expected to be EOT + CRC (2 bytes)
      return true;
    }
  }
  return false;
}

bool SerialDataLink::checkCRC() 
{
  if (receiveBufferIndex < 3) 
  {
    // Not enough data for CRC check
    return false;
  }

  // Extract CRC from the last two bytes of the packet
  uint16_t receivedCrc = (uint16_t(receiveBuffer[receiveBufferIndex - 2]) << 8) 
                         | uint16_t(receiveBuffer[receiveBufferIndex - 1]);

  // Calculate CRC for the received data (excluding the CRC bytes themselves)
  uint16_t calculatedCrc = calculateCRC16(receiveBuffer, receiveBufferIndex - 2);

  return receivedCrc == calculatedCrc;
}

void SerialDataLink::processPacket() 
{
  if (!checkCRC()) {
    // CRC check failed, handle the error
    readError = true;
    return;
  }

  // Start from index 2 to skip the SOT and ADDRESS characters
  uint8_t i = 2; 

  // Check if the next character is not the EOT character
  while (receiveBuffer[i] != eotChar) {
    uint8_t arrayID = receiveBuffer[i++];
    
    // Make sure there's enough data for a complete int16 (2 bytes)
    if (i + 1 >= receiveBufferIndex) {
      readError = true;
      return; // Incomplete packet or buffer overflow
    }

    // Combine the next two bytes into an int16 value
    int16_t value = (int16_t(receiveBuffer[i]) << 8) | int16_t(receiveBuffer[i + 1]);
    i += 2;

    // Handle the array ID and value here
    // For example, updating the corresponding value in dataArray
    if (arrayID < dataArraySize) {
      dataArray[arrayID] = value;
    } else {
      // Handle invalid array ID
      readError = true;
      return;
    }
    newData = true;
    sendACK();
  }

  // Post-processing after successfully reading the packet
  // For example, setting flags or calling callbacks
}


void SerialDataLink::sendACK() 
{
  serial.write(ackChar);
}

bool SerialDataLink::checkReadError(bool reset) 
{
  bool error = readError;
  if (reset) {
    readError = false;
  }
  return error;
}

uint16_t SerialDataLink::calculateCRC16(const uint8_t* data, size_t length)
{
  uint16_t crc = 0xFFFF; // Start value for CRC
  for (size_t i = 0; i < length; i++)
  {
    uint8_t index = (crc >> 8) ^ data[i];
    crc = (crc << 8) ^ crcTable[index];
  }
  return crc;
}
