/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <EmonLib.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);


//NABITO SETTINGS
EnergyMonitor emon1; // Create an instance
int voltage = 230;                //nominal AC socket voltage
float freq = 2;   //freq = monitor AC current every x seconds
int analogInputPin = 0;   //analog input pin for current monitoring

//aux variables
String loadValues;
unsigned long lastTime = 0;


void setup()
{
  Serial.begin(57600);
 
  nfc.begin();
  emon1.current(analogInputPin, 30);   // Current: input pin, calibration.
}

void loop()
{
    unsigned long now = millis();
    if (now - lastTime > (freq*1000)) {
      lastTime = now;

     // Serial.println("\nScan a NFC tag\n");
     if (nfc.tagPresent()) {
      readTag(); 
     }
    
      double Irms = emon1.calcIrms(1480);  // Calculate Irms only, 1480 is the number of cycles
      if ( Irms < 0.20 ) {   //Arduino's ADC is not very precise at very low voltage values, therfore truncating to 0A if I>0.2A
        Irms = 0.0;
      }
      double pwr  = Irms * voltage;        // calculate power
      
      loadValues = "{\"LOAD\":{\"I\":";
      loadValues = loadValues + Irms + ",\"V\":" + voltage + ",\"P\":" + pwr + "}}";  //put everything into a JSON string

      Serial.println(loadValues); //print JSON to serial
    }
  
}


void readTag() {
    NfcTag tag = nfc.read();
    //Serial.println(tag.getTagType());
    //Serial.print("UID: ");Serial.println(tag.getUidString());

    if (tag.hasNdefMessage()) // every tag won't have a message
    {
      
      NdefMessage message = tag.getNdefMessage();
      
      /*Serial.print("\nThis NFC Tag contains an NDEF Message with ");
      Serial.print(message.getRecordCount());
      Serial.print(" NDEF Record");
      if (message.getRecordCount() != 1) {
        Serial.print("s");
      }
      Serial.println(".");
      */

      // cycle through the records, printing some info from each
      int recordCount = message.getRecordCount();
      if (message.getRecordCount() == 1) { //#TODO temporary workaround
        // cycle through the records, printing some info from each
        char *pub;
        char *pri;
        int recordCount = message.getRecordCount();
        
          //Serial.print("\nNDEF Record ");Serial.println(i+1);
          NdefRecord record = message.getRecord(0);
        
          int payloadLength = record.getPayloadLength();
          byte payload[payloadLength];
          record.getPayload(payload);
  
        
          //transform string to char array    
          char payBuf[payloadLength-1];
          int j = 0;
          for (int c = 3; c < payloadLength; c++) {  // skip first 3 bytes - because '.en' kept appearing there, I don't know why :|
            payBuf[j] = (char)payload[c];
            j++;
          }
          payBuf[j] = '\0';
         
          
          //Serial.print("{\"NFC\":\"");
          Serial.println(payBuf);
          //Serial.println("\"}");
  
          // id is probably blank and will return ""
          /*
          String uid = record.getId();
          if (uid != "") {
            Serial.print("  ID: ");Serial.println(uid);
          }
          */
        } // for cycle
      } // end of if
  delay(2000);
}


