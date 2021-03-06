#define SEP '|' // |
#define SOSPIN 6
#define LEADERPIN 9
#define ALTITUDE 1005.0
#define SENSOR_MS 10 * 1000


#include <string.h>
#include <SoftwareSerial.h>
#include <SFE_BMP180.h>
#include <Wire.h>

SoftwareSerial GSMSerial(11, 10); // RX, TX
SoftwareSerial bt(7,8);

const char GPRS1[] = "AT+SAPBR=3,1,\"Contype\",\"GPRS\"";
const char GPRS2[] = "AT+SAPBR=3,1,\"APN\",\"internet\"";
const char GPRS3[] = "AT+SAPBR=1,1";
const char GPRS4[] = "AT+SAPBR=2,1";
const char HTTP1[] = "AT+HTTPINIT";
const char HTTP2[] = "AT+HTTPPARA=\"CID\",1";
const char HTTP3[] = "AT+HTTPPARA=\"URL\",\"http://m.oguz.lol/node?data=";
const char HTTP4[] = "AT+HTTPACTION=0";

const int DEBUG = 0;
const int IGNITE_PIN = 5;

boolean IS_LEADER;
char sosMessage[8];
char sensorMessage[18];

char inBluetooth = -1;
char inBtBuffer[100];
int btPos = 0;
boolean gotBtMessage;

char inHw = -1;
char inHwBuffer[100];
int hwPos = 0;
boolean gotHwMessage;

int sos, lastSos;
int newStringPos = 0;

char altArr[4];
char tempArr[3];
int dd = 0;
char status;
double T,P,p0,a;

SFE_BMP180 pressure;

unsigned long lastSensorSendTime;
unsigned long lastWarningLedTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  bt.begin(9600);
  GSMSerial.begin(19200);
  igniteGSM();
  pinMode(SOSPIN, INPUT);
  pinMode(LEADERPIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  lastSos = LOW;
  sos = LOW;
  lastSensorSendTime = 0;
  lastWarningLedTime = 0;

  delay(5000);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(400);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);

  IS_LEADER = digitalRead(LEADERPIN);
  digitalWrite(LED_BUILTIN, HIGH);

  while(GSMSerial.available()) {
    if (DEBUG) {
      Serial.write(GSMSerial.read());
    } else {
      GSMSerial.read();
    }
  }

  turnOffEcho();
  openAttachGPRS();

  digitalWrite(LED_BUILTIN, LOW);
  delay(200);

  if (pressure.begin()) {
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(400);
    digitalWrite(LED_BUILTIN, LOW);
    if (IS_LEADER) {
      delay(100);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(400);
      digitalWrite(LED_BUILTIN, LOW);
    }
       // Serial.println("BMP180 init success");
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }

}

void loop() {

  sos = digitalRead(SOSPIN);

  readBluetooth();
  readHardware();

  readSensor();

  if (millis() - lastWarningLedTime > 200) {
    digitalWrite(LED_BUILTIN, LOW);
  }


   if (sos == HIGH && lastSos != HIGH) {
      sendSosSignal();
      lastSos = sos;
      sos = LOW;
      digitalWrite(LED_BUILTIN, HIGH);
      lastWarningLedTime = millis();
   }
   else {
      lastSos = sos;
   }

    if(gotBtMessage) {
      useBtMessage();
      clearBtBuffer();
   }

   if(gotHwMessage){
      useHwMessage();
      clearHwBuffer();

   }
}
void useBtMessage(){
    if (inBtBuffer[0] != '$' || inBtBuffer[1] != '$'  || inBtBuffer[2] != '$' ||
        inBtBuffer[btPos-2] != '$' || inBtBuffer[btPos-3] != '$' || inBtBuffer[btPos-4] != '$') {
        digitalWrite(LED_BUILTIN, HIGH);
        lastWarningLedTime = millis();
        sendBtHumanMessage("Could not parse bt");
        return;
    }
  // if this message is from a human
  if(inBtBuffer[3] == '1' || inBtBuffer[3] == '2' || inBtBuffer[3] == '3' || inBtBuffer[3] == '4'){
    forwardBtToHw();
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
    lastWarningLedTime = millis();
    return;
  }
}

void useHwMessage(){
    if (inHwBuffer[0] != '$' || inHwBuffer[1] != '$'  || inHwBuffer[2] != '$' ||
        inHwBuffer[hwPos-2] != '$' || inHwBuffer[hwPos-3] != '$' || inHwBuffer[hwPos-4] != '$') {
        digitalWrite(LED_BUILTIN, HIGH);
        lastWarningLedTime = millis();
        sendBtHumanMessage("Could not parse hw");
        return;
    }
  // if this message is from a human
  if(inHwBuffer[3] == '1' || inHwBuffer[3] == '2' || inHwBuffer[3] == '3' || inHwBuffer[3] == '4'){
    forwardHwToBt();
    // TODO LEADER
    if (IS_LEADER) {
        inHwBuffer[hwPos+1] = '\0';
        makeHTTPRequest(inHwBuffer);
    }
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
    lastWarningLedTime = millis();
  }
}

//To forward the human message from a node to the other node
void forwardBtToHw(){
    inBtBuffer[4] = '2';
    sendHw(inBtBuffer, btPos - 1);
}

//To deliver the human message from a node to a phone
void forwardHwToBt(){
    inHwBuffer[4] = '3';
    sendBt(inHwBuffer, hwPos - 1);
}
void sendSensorValues(){
  sensorMessage[0] = '$';
  sensorMessage[1] = '$';
  sensorMessage[2] = '$';
  sensorMessage[3] = '3';
  sensorMessage[4] = '2';
  sensorMessage[5] = tempArr[0];
  sensorMessage[6] = tempArr[1];
  sensorMessage[7] = '.';
  sensorMessage[8] = tempArr[2];
  sensorMessage[9] = '#';
  sensorMessage[10] = altArr[0];
  sensorMessage[11] = altArr[1];
  sensorMessage[12] = altArr[2];
  sensorMessage[13] = altArr[3];
  sensorMessage[14] = '$';
  sensorMessage[15] = '$';
  sensorMessage[16] = '$';
  sensorMessage[17] = '\0';
  sendBt(sensorMessage, 17);
  sendHw(sensorMessage, 17);
  if (IS_LEADER) {
    makeHTTPRequest(sensorMessage);
  }
}
void sendSosSignal() {
  sosMessage[0] = '$';
  sosMessage[1] = '$';
  sosMessage[2] = '$';
  sosMessage[3] = '2';
  sosMessage[4] = '2';
  sosMessage[5] = '$';
  sosMessage[6] = '$';
  sosMessage[7] = '$';

  sendBt(sosMessage, 8);
  sendHw(sosMessage, 8);
//  Serial.println(sosMessage);
//  bt.print(sosMessage);

}

void readSensor() {
  if (millis() - lastSensorSendTime < SENSOR_MS) {
    return;
  }
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    status = status + 10;

    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
         delay(status);
         status = pressure.getPressure(P,T);
         if (status != 0)
         {
            p0 = pressure.sealevel(P,ALTITUDE);
            a = pressure.altitude(P,p0); // metre
            //Height Char Calculations
            int alt1 = a / 1000;
            int alt2 = (a / 100) - (alt1 * 10);
            int alt3 = (a / 10) - (alt1 * 100) - (alt2 * 10);
            int alt4 = ((int)a) % 10;

            altArr[0] = '0' + char (alt1);
            altArr[1] = '0' + char (alt2);
            altArr[2] = '0' + char (alt3);
            altArr[3] = '0' + char (alt4);


            // Temp Char Calculations
            int tempValue = analogRead(A0);
            float temp = tempValue * 0.48828125;
            int tempInt = temp;
            int temp1 = temp / 10;
            int temp2 = temp - (temp1 * 10);
            int temp3 = (temp  * 10) - (tempInt * 10);

            tempArr[0] = '0' + char(temp1);
            tempArr[1] = '0' + char(temp2);
            tempArr[2] = '0' + char(temp3);

            lastSensorSendTime = millis();
           sendSensorValues();
         }
      }
    }
  }
}

void readBluetooth() {
  if (bt.available()) {
    inBluetooth = bt.read();
  }
  else {
    inBluetooth = -1;
  }

  if((inBluetooth >= 0) && (inBluetooth <= 127)){
      inBtBuffer[btPos++] = inBluetooth;
   }

   gotBtMessage = inBluetooth == SEP;
}

void readHardware() {
  if(Serial.available()) {
    inHw = Serial.read();
  }
  else {
    inHw = -1;
  }

  if((inHw >= 0) && (inHw <= 127)){
      inHwBuffer[hwPos++] = inHw;
    }
   gotHwMessage = inHw == SEP;
}

void clearBtBuffer() {
  for (int c = 0; c < btPos; c++){
        inBtBuffer[c] = 0;
      }
      btPos = 0;
      inBluetooth = -1;
}

void clearHwBuffer() {
  for (int c = 0; c < hwPos; c++){
        inHwBuffer[c] = 0;
      }
      hwPos = 0;
      inHw = -1;

}

void sendBt(char buffer[], int len) {
  for (int i=0;i<len;i++) {
    if (buffer[i] <= 0 || buffer[i] > 127) {
      break;
    }
    bt.print(buffer[i]);
  }
  bt.print(SEP); //
  // if (SEP == '|') {
  //   bt.print('\n');
  // }
}

void sendHw(char buffer[], int len) {
  for (int i=0;i<len;i++) {
    if (buffer[i] <= 0 || buffer[i] > 127) {
      break;
    }
    Serial.print(buffer[i]);
  }
  Serial.print(SEP);
}

void sendBtHumanMessage(char str[]) {
    char buf[24];
    int c = 0;
    while(str[c] != '\0') {
        buf[5+c] = str[c];
        c++;
    }
    buf[0] = '$';
    buf[1] = '$';
    buf[2] = '$';
    buf[3] = '1';
    buf[4] = '3';
    buf[5+c] = '$';
    buf[5+c+1] = '$';
    buf[5+c+2] = '$';
    sendBt(buf, 4+c+3);
}

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
  sendATCommand(HTTP4);
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


