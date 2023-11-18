# SerialDataLink

SerialDataLink is a half-duplex serial variables transfer library for Arduino, designed for efficient and reliable, automatic transmission of int16 variables between controllers. It uses a non-blocking, polling-based approach, suitable for scenarios that require continuous monitoring and variable data transfer.

## Features

- **Half-Duplex Communication**: Allows bidirectional data transfer but not simultaneously.
- **Non-Blocking Operation**: Uses a polling mechanism for data handling.
- **Error Checking**: Incorporates transmission and reception error checks.
- **Flexible Data Handling**: Transmits only the variables which values have change or reached update time..

### Example

```
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
```

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
