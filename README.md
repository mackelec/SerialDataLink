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

## Public Methods

| Method | Parameters | Description |
| ------ | ---------- | ----------- |
| `SerialDataLink` | `Stream &serial`, `uint8_t transmitID`, `uint8_t receiveID`, `uint8_t maxIndexTX`, `uint8_t maxIndexRX`, `bool enableRetransmit` | Constructor to initialize the communication parameters. |
| `run` | - | Main method to be called frequently to handle data transmission and reception. |
| `updateData` | `uint8_t index`, `int16_t value` | Updates data to be transmitted. |
| `getReceivedData` | `uint8_t index` | Retrieves data received from the serial link. |
| `checkNewData` | `bool resetFlag` | Checks if new data has been received. |
| `checkTransmissionError` | `bool resetFlag` | Checks for transmission errors. |
| `getLastAcknowledge` | Gets the status of the last acknowledgment. | `bool resetFlag`: Set to `true` to reset the acknowledgment status after checking. | `int`: Returns the last acknowledgment status. |
| `checkReadError` | `bool resetFlag` | Checks for read errors. |
| `setUpdateInterval` | `unsigned long interval` | Sets the interval for data updates. |
| `setAckTimeout` | `unsigned long timeout` | Sets the timeout for acknowledgments. |
| `setPacketTimeout` | `unsigned long timeout` | Sets the timeout for packet reception. |
| `muteACK` | `bool mute` | Mutes or unmutes the acknowledgment messages |
| `setHeaderChar` | `char header` | Sets the character used to denote the start of a packet. |
| `setEOTChar` | `char eot` | Sets the character used to denote the end of a packet. |



## License

This project is licensed under the MIT License - see the LICENSE file for details.

