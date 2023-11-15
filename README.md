# SerialDataLink Library for Arduino

## Overview
The SerialDataLink library offers robust serial communication for Arduino, ideal for reliable point-to-point data transfer. This library features enhanced transmission controls, acknowledgment handling, and error management, making it suitable for various applications.

## Features
- **Transmission on Data Change**: Sends data when there's a change or at a predetermined interval.
- **ACK Handling**: ACKs are sent for packets with correct data and CRC.
- **Timeout Management**: 
  - **Transmitter Timeout**: Detects lack of ACK reception within a specific timeframe.
  - **RX Frame Timeout Error**: Manages timeout if excessive time passes receiving a packet, indicating potential communication issues.
- **Customizable Update and Timeout Intervals**: Allows setting specific intervals for data updates and timeouts.
- **CRC Validation**: Ensures the integrity of transmitted data.
- **Automatic Retransmission Option**: For increased reliability.
- **Buffer Overflow Management**: Prevents data loss due to buffer capacity issues.

## Methods

### Constructor
- `SerialDataLink(Stream &serial, uint8_t address, uint8_t maxIndex, bool enableRetransmit = false)`
  - Initializes a new SerialDataLink object.
  - `serial`: Reference to the serial stream.
  - `address`: Address of the device in the network.
  - `maxIndex`: Maximum index for the data array.
  - `enableRetransmit`: Enables or disables automatic retransmission.

### Transmission Control
- `void run()`
  - Main method for handling data transmission.
- `void updateData(uint8_t index, int16_t value)`
  - Updates the data at a specified index.
  - `index`: Index of the data to update.
  - `value`: New value to be set.
- `bool checkTransmissionError(bool resetFlag)`
  - Checks for transmission errors.
  - `resetFlag`: Resets the error flag if set to true.

### Configuration Setters
- `void setHeaderChar(char header)`
  - Sets the header character for packets.
  - `header`: Character representing the packet start.
- `void setEOTChar(char eot)`
  - Sets the End-of-Transmission character.
  - `eot`: Character representing the end of a packet.
- `void setACKChar(char ack)`
  - Sets the Acknowledgment character.
  - `ack`: Character used for acknowledgment.
- `void setUpdateInterval(unsigned long interval)`
  - Sets the interval for updating data values.
  - `interval`: Time in milliseconds.
- `void setAckTimeout(unsigned long timeout)`
  - Sets the timeout for ACK reception.
  - `timeout`: Time in milliseconds.
- `void setPacketTimeout(unsigned long timeout)`
  - Sets the timeout for packet reception.
  - `timeout`: Time in milliseconds.

### Reception Control
- `void read()`
  - Main method for handling data reception.
- `int16_t readData(uint8_t index)`
  - Reads data at a specified index.
  - `index`: Index of the data to read.
- `bool checkNewData(bool resetFlag)`
  - Checks for the arrival of new data.
  - `resetFlag`: Resets the new data flag if set to true.
- `bool checkReadError(bool reset)`
  - Checks for read errors.
  - `reset`: Resets the read error flag if set to true.



## Usage Example
```
#include "SerialDataLink.h"

// Initialize SerialDataLink for transmission and reception
SerialDataLink dataLinkTransmit(Serial2, 0x01, 10);
SerialDataLink dataLinkReceive(Serial3, 0x01, 10);

void setup() {
  Serial.begin(115200); // Start the Serial Monitor
  Serial2.begin(9600);  // Transmission setup
  Serial3.begin(9600);  // Reception setup

  // Initialize data for transmission
  for (uint8_t i = 0; i < 10; i++) {
    dataLinkTransmit.updateData(i, i * 10);
  }
}

void loop() {
  // Regularly update and transmit data
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    for (uint8_t i = 0; i < 10; i++) {
      dataLinkTransmit.updateData(i, random(100));
    }
  }

  // Execute transmission and reception routines
  dataLinkTransmit.run();
  dataLinkReceive.read();

  // Check and handle transmission and read errors
  bool sendError = dataLinkTransmit.checkTransmissionError(true);
  bool readError = dataLinkReceive.checkReadError(true);

  // Process newly received data
  if (dataLinkReceive.checkNewData(true)) {
    for (uint8_t i = 0; i < 10; i++) {
      int16_t data = dataLinkReceive.readData(i);
      // Further processing...
    }
  }
}
```



## License
This project is licensed under the MIT License.
