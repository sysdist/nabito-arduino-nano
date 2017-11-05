# Nabito Arduino module sketch

This sketch runs on Arduino Nano in the Nabito control box, but you can run it on Arduino Uno or any other Arduino Uno/Nano compatible board.

It monitors the AC current on the mains cable via the CT monitor and manages the NFC sensor (chip from Elechouse PN532).
Both the AC load values and the NFC sensors publish data to the serial port as JSON values, e.g:

```
{
 "NFC": {
    "user": "user-id-123456",
    "token": "token-random123456id7890"
  },
  "LOAD": {
    "I": 8.44,
    "V": 230,
    "P": 1941.66
  }
}
```
