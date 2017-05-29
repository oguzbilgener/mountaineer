#include <SoftwareSerial.h>
#include <string.h>

SoftwareSerial GSMSerial(11, 10); // RX, TX
const int DEBUG = 1;
const int IGNITE_PIN = 5;
const char GPRS1[] = "AT+SAPBR=3,1,\"Contype\",\"GPRS\"";
const char GPRS2[] = "AT+SAPBR=3,1,\"APN\",\"internet\"";
const char GPRS3[] = "AT+SAPBR=1,1";
const char GPRS4[] = "AT+SAPBR=2,1";
const char HTTP1[] = "AT+HTTPINIT";
const char HTTP2[] = "AT+HTTPPARA=\"CID\",1";
const char HTTP3[] = "AT+HTTPPARA=\"URL\",\"http://m.oguz.lol/node?data=";
const char HTTP4[] = "AT+HTTPACTION=0";

int sendATCommand(const char *cmd, int del = 500, char *data = NULL) {
  char response[64];
  int index = 0;

  //url encoding
  if (data) {
    GSMSerial.print(cmd);
    
    const char *cursor_;
    cursor_ = data;
    while (*cursor_) {
      switch (*cursor_) {
        case '$':
          GSMSerial.print("%24");
          break;
        case '#':
          GSMSerial.print("%23");
          break;
        case '|':
          GSMSerial.print("%7C");
          break;
        default: 
          GSMSerial.print(*cursor_);
      }
      cursor_++;
    }
    GSMSerial.print("\"\r\n");
  } else {
    GSMSerial.println(cmd);
  }

  delay(del);
  
  while (GSMSerial.available()) {
    response[index++] = GSMSerial.read();
  }

  //null
  response[index] = '\0';

  if (DEBUG) {
    Serial.print("CMD: ");
    Serial.println(cmd);
    if (data) {
      Serial.print("Data: ");
      Serial.println(data);
    }
    Serial.print("Response: ");
    Serial.println(response);
  }

  if ( strcmp(response, "\r\nOK\r\n") == 0 ) {
    return 1;
  } else if ( strcmp(response, "\r\nERROR\r\n") == 0 ) {
    return 0;
  } else {
    return -1;
  }
}

void turnOffEcho() {
  GSMSerial.println("ATE0");
  delay(500);
  while(GSMSerial.available()) {
    if (DEBUG) {
      Serial.write(GSMSerial.read());
    } else {
      GSMSerial.read();
    }
  }
}

int makeHTTPRequest(char *data) {  
  sendATCommand(HTTP1);
  sendATCommand(HTTP2);
  sendATCommand(HTTP3, 500, data);
  sendATCommand(HTTP4, 5000);
}

void openAttachGPRS() {
  sendATCommand(GPRS1);
  sendATCommand(GPRS2);
  while (sendATCommand(GPRS3, 3000) != 1) {}
  sendATCommand(GPRS4);
}

void igniteGSM() {
  pinMode(IGNITE_PIN, OUTPUT);
  delay(3000);
  pinMode(IGNITE_PIN, INPUT);
}

void setup() {
  pinMode(IGNITE_PIN, OUTPUT);
  
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  GSMSerial.begin(19200);
  igniteGSM();
  delay(5000);
  
  while(GSMSerial.available()) {
    if (DEBUG) {
      Serial.write(GSMSerial.read());
    } else {
      GSMSerial.read();
    }
  }
  
  turnOffEcho();
  openAttachGPRS();

  char ptr[9] = "$$$23$$$";
  makeHTTPRequest(ptr);
}

// the loop function runs over and over again forever
void loop() {
  if (DEBUG && GSMSerial.available()) {
      Serial.write(GSMSerial.read());
  }

  if (DEBUG && Serial.available()) {
    char outgoing = Serial.read();
    GSMSerial.write(outgoing);
    Serial.write(outgoing);
  }                   
}  
