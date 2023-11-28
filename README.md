# SerialDataLink

SerialDataLink is a half-duplex serial - "variables transfer" library for Arduino, designed for efficient and reliable, automatic transmission of int16 variables between controllers. It uses a non-blocking, polling-based approach, suitable for scenarios that require continuous monitoring and variable data transfer.

## Features

- **Half-Duplex Communication**: Allows half duplex bidirectional data transfer.
- **Non-Blocking Operation**: Uses a polling mechanism for data handling.
- **Error Checking**: Incorporates transmission and reception CRC16 data and framing error checks.
- **Flexible Data Handling**: Transmits only the variables which values have change or reached update time..

### Example

```
#include <SerialDataLink.h>

HardwareSerial Serial2(PA3, PA2); // RX, TX
HardwareSerial Serial3(PB11, PB10); // RX, TX


#define PRIMARY_VARS_SEND     20
#define SECONDARY_VARS_SEND   5

#define PRIMARY_VARS_RECV     SECONDARY_VARS_SEND
#define SECONDARY_VARS_RCV    PRIMARY_VARS_SEND

//                                              /*     Sending Vars       Receiving Vars */
// Create two instances of SerialDataLink
SerialDataLink dataLinkPrimary(Serial2, 0x01 ,0 ,    PRIMARY_VARS_SEND ,  PRIMARY_VARS_RECV);
SerialDataLink dataLinkSecondary (Serial3, 0,  0x01, SECONDARY_VARS_SEND ,SECONDARY_VARS_RCV);

#define LED   PC13

void setup() {
  Serial.begin(115200);    // Start the Serial Monitor
  Serial2.begin(9600); // Initialize Serial2 for transmission
  Serial3.begin(9600); // Initialize Serial3 for reception

  pinMode(LED,OUTPUT);



  // Initialize data to be sent
  for (uint8_t i = 0; i < PRIMARY_VARS_SEND; i++) {
    dataLinkPrimary.updateData(i, 3+i*3 ); // Example initial values
  }
  for (uint8_t i = 0; i < SECONDARY_VARS_SEND; i++) {
    dataLinkSecondary.updateData(i, 10 + i*5 ); // Example initial values
  }
}


void loop() {
  // Update data every second
  
  static unsigned long lastUpdate = 0;
  static unsigned long lastCycle = 0;

  
  if (millis() - lastUpdate >= 3000) 
  {
    digitalToggle(LED);
    Serial.println("Hello from stm32F105");
    lastUpdate = millis();
    for (uint8_t i = 0; i < PRIMARY_VARS_SEND; i++) {
      dataLinkPrimary.updateData(i,i*20 /*random(100)*/); // Update with random values
    }
    for (uint8_t i = 0; i < SECONDARY_VARS_SEND; i++) {
      dataLinkSecondary.updateData(i,100+i*50 /*random(100)*/); // Update with random values
    }
  }

  // Run transmit and receive methods every millisecond
  
  static unsigned long lastMillis = 0;
  static unsigned long last100Millis = 0;
  
  if (millis() != lastMillis ) 
  {
    lastMillis = millis();
    /*
    *    run() should be polled at about the serial bit rate 
    */
    dataLinkPrimary.run();
    dataLinkSecondary.run();
  }  

  if (millis() - last100Millis >= 100) 
  {
    last100Millis = millis();
    bool sendError = dataLinkPrimary.checkTransmissionError(true);
    if (sendError) Serial.println("SEND error - Primary");
    bool readError = dataLinkSecondary.checkReadError(true);
    if (readError) Serial.println("read error - Secondary");

    sendError = dataLinkSecondary.checkTransmissionError(true);
    if (sendError) Serial.println("SEND error - Secondary");
    readError = dataLinkPrimary.checkReadError(true);
    if (readError) Serial.println("read error - Primary");
  }

   //Check for received data
  if (dataLinkSecondary.checkNewData(true)) 
  {
    Serial.println("New data received: Secondary");
    for (uint8_t i = 0; i < SECONDARY_VARS_RCV; i++) 
    {
      Serial.print("Index ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(dataLinkSecondary.getReceivedData(i));
    }
  }
  if (dataLinkPrimary.checkNewData(true)) 
  {
    Serial.println("New data received: Primary");
    for (uint8_t i = 0; i < PRIMARY_VARS_RECV; i++) 
    {
      Serial.print("Index ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(dataLinkPrimary.getReceivedData(i));
    }
  }
}
```
## Constructor

### `SerialDataLink(Stream &serial, uint8_t transmitID, uint8_t receiveID, uint8_t maxIndexTX, uint8_t maxIndexRX, bool enableRetransmit)`

This is the constructor for the `SerialDataLink` class. It initializes the communication parameters with the following details:

- `serial`: The serial stream used for communication.
- `transmitID`: The identifier used for transmission.
- `receiveID`: The identifier used for receiving data.
- `maxIndexTX`: The maximum index that can be used for transmitting data.
- `maxIndexRX`: The maximum index that can be used for receiving data.
- `enableRetransmit`: A boolean flag that enables retransmission if set to `true`.

## Public Methods

| Method | Description and Parameters |
| ------ | -------------------------- |
| `void run()` | Main method for data transmission and reception. |
| `void updateData(uint8_t index, int16_t value)` | Updates data to be transmitted.<br> - `index`: Data position index.<br> - `value`: Data value to send. |
| `int16_t getReceivedData(uint8_t index)` | Retrieves received data.<br> - `index`: Index of data to retrieve. |
| `bool checkNewData(bool resetFlag)` | Checks if new data is received.<br> - `resetFlag`: Clears new data flag if true. |
| `bool checkTransmissionError(bool resetFlag)` | Checks for transmission errors.<br> - `resetFlag`: Resets error flag if true. |
| `int getLastAcknowledge(bool resetFlag)` | Gets the status of the last acknowledgment.<br> - `resetFlag`: Resets acknowledgment status if true. |
| `bool checkReadError(bool resetFlag)` | Checks for read errors.<br> - `resetFlag`: Resets error flag if true. |
| `void setUpdateInterval(unsigned long interval)` | Sets the data update interval.<br> - `interval`: Time in milliseconds. |
| `void setAckTimeout(unsigned long timeout)` | Sets the acknowledgment timeout.<br> - `timeout`: Timeout duration in milliseconds. |
| `void setPacketTimeout(unsigned long timeout)` | Sets the packet reception timeout.<br> - `timeout`: Timeout period in milliseconds. |
| `void muteACK(bool mute)` | Mutes or unmutes acknowledgment messages.<br> - `mute`: Mute flag. |
| `void setHeaderChar(char header)` | Sets the start-of-packet character.<br> - `header`: Character for packet start. |
| `void setEOTChar(char eot)` | Sets the end-of-transmission character.<br> - `eot`: Character for end of transmission. |





## License

This project is licensed under the MIT License - see the LICENSE file for details.

