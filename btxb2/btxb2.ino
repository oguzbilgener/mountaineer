#define SEP '|' // |
#define SOSPIN 6
#define LEADERPIN 9
#define ALTITUDE 1005.0
#define SENSOR_MS 10 * 1000


#include <SoftwareSerial.h>
#include <SFE_BMP180.h>
#include <Wire.h>

SoftwareSerial bt(7,8);

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
  pinMode(SOSPIN, INPUT);
  pinMode(LEADERPIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  lastSos = LOW;
  sos = LOW;
  lastSensorSendTime = 0;
  lastWarningLedTime = 0;

  digitalWrite(LED_BUILTIN, HIGH);
  delay(400);
  digitalWrite(LED_BUILTIN, LOW);

  IS_LEADER = digitalRead(LEADERPIN);

  if (pressure.begin()) {
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(400);
    digitalWrite(LED_BUILTIN, LOW);
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
        return;
    }
  // if this message is from a human
  if(inBtBuffer[3] == '1' || inBtBuffer[3] == '2' || inBtBuffer[3] == '3' || inBtBuffer[3] == '3'){
    forwardBtToHw();
  }
}

void useHwMessage(){
    if (inHwBuffer[0] != '$' || inHwBuffer[1] != '$'  || inHwBuffer[2] != '$' ||
        inHwBuffer[hwPos-2] != '$' || inHwBuffer[hwPos-3] != '$' || inHwBuffer[hwPos-4] != '$') {
        digitalWrite(LED_BUILTIN, HIGH);
        lastWarningLedTime = millis();
        return;
    }
  // if this message is from a human
  if(inHwBuffer[3] == '1' || inHwBuffer[3] == '2' || inHwBuffer[3] == '3' || inHwBuffer[3] == '4'){
    forwardHwToBt();
    // TODO LEADER
    if (IS_LEADER) {

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


