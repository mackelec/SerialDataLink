# SerialDataLink

SerialDataLink is a half-duplex serial communication library for Arduino, designed for efficient and reliable data transmission between controllers. It uses a non-blocking, poll-based approach, suitable for scenarios that require continuous monitoring and variable data transfer.

## Features

- **Half-Duplex Communication**: Allows bidirectional data transfer but not simultaneously.
- **Non-Blocking Operation**: Uses a polling mechanism for data handling.
- **Error Checking**: Incorporates transmission and reception error checks.
- **Flexible Data Handling**: Supports variable data sizes for transmission and reception.

## Installation

1. Download the library as a zip.
2. In the Arduino IDE, navigate to Sketch > Include Library > Add .ZIP Library.
3. Select the downloaded zip file.
4. Restart the Arduino IDE.

## Usage

Include the `SerialDataLink` library in your Arduino sketch and follow these steps:

1. Define `SerialDataLink` objects for each communication channel.
2. Initialize the objects with appropriate parameters.
3. Call the `run()` method regularly within the `loop()` function.
4. Use other public methods as required for data transmission and reception.

### Example

[CODEBLOCK_START]
#include "SerialDataLink.h"

// Define SerialDataLink object
SerialDataLink dataLink(Serial2, 1, 0, 10, 10);

void setup() {
  // Initialize Serial and DataLink
  Serial.begin(115200);
  Serial2.begin(9600);
}

void loop() {
  // Regularly call run() method
  dataLink.run();

  // Additional code...
}
[CODEBLOCK_END]

## Public Methods

| Method | Parameters | Description |
| ------ | ---------- | ----------- |
| `SerialDataLink` | `Stream &serial`, `uint8_t transmitID`, `uint8_t receiveID`, `uint8_t maxIndexTX`, `uint8_t maxIndexRX`, `bool enableRetransmit` | Constructor to initialize the communication parameters. |
| `run` | - | Main method to be called frequently to handle data transmission and reception. |
| `updateData` | `uint8_t index`, `int16_t value` | Updates data to be transmitted. |
| `getReceivedData` | `uint8_t index` | Retrieves data received from the serial link. |
| `checkNewData` | `bool resetFlag` | Checks if new data has been received. |
| `checkTransmissionError` | `bool resetFlag` | Checks for transmission errors. |
| `checkReadError` | `bool resetFlag` | Checks for read errors. |
| `setUpdateInterval` | `unsigned long interval` | Sets the interval for data updates. |
| `setAckTimeout` | `unsigned long timeout` | Sets the timeout for acknowledgments. |
| `setPacketTimeout` | `unsigned long timeout` | Sets the timeout for packet reception. |
| `setHeaderChar` | `char header` | Sets the character used to denote the start of a packet. |
| `setEOTChar` | `char eot` | Sets the character used to denote the end of a packet. |

## Contributing

Contributions to the `SerialDataLink` library are welcome. Please follow the standard GitHub pull request workflow. Ensure your code adheres to the existing style and all tests pass.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## License
This project is licensed under the MIT License.
