#define MY_NODE_ID 50
#define CHILD_ID_POWER_DELIVERED 13
#define CHILD_ID_METER_READING 14

//    13: ('POWER_W', 'V_WATT'),  # Actual electricity power delivered (+P) in kW
//    14: ('POWER_KWH', 'V_KWH'),  # Meter Reading electricity delivered to client in kWh

#define MY_DEBUG 
#define MY_RADIO_NRF24

#include <AltSoftSerial.h>
#include <MySensors.h> 

// AltSoftSerial always uses these pins:
//
// Board          Transmit  Receive   PWM Unusable
// -----          --------  -------   ------------
// Teensy 2.0         9        10       (none)
// Teensy++ 2.0      25         4       26, 27
// Arduino Uno        9         8         10
// Arduino Mega      46        48       44, 45
// Wiring-S           5         6          4
// Sanguino          13        14         12
 
AltSoftSerial altSerial;
char c;
String currentLine = "";

MyMessage msgCurrentPower(CHILD_ID_POWER_DELIVERED, V_WATT);
MyMessage msgTotalPower(CHILD_ID_METER_READING, V_KWH);
 
void setup() {
    Serial.begin(115200);
    altSerial.begin(115200);
}

void presentation() {
  sendSketchInfo("SmartMeterNode", "1.0");
  present(CHILD_ID_POWER_DELIVERED, V_WATT);
  present(CHILD_ID_METER_READING, V_KWH);
}
 
void loop() {
    if (altSerial.available()) {
        c = altSerial.read();
   
        // --- 7 bits ---
        c &= ~(1 << 7);
        char inChar = (char)c;
        if(inChar == '\n'){
          if(currentLine.startsWith("1-0:1.7.0")){
            
            // Serial.println("KW:" + String(extractValue(currentLine)));
            send(msgCurrentPower.set(extractValue(currentLine),1));      

            
          } else if (currentLine.startsWith("1-0:1.8.1")){
            send(msgTotalPower.set(extractValue(currentLine),1));      
            // Serial.println("KWH:" + String(extractValue(currentLine)));
          }
          currentLine = "";
        } else {
          currentLine += c;
        }
 
    }
}

float extractValue(String line){
  int startIndex = currentLine.lastIndexOf("(") + 1;
  int endIndex = currentLine.lastIndexOf("*"); 
  return line.substring(startIndex,endIndex).toFloat();  
}


