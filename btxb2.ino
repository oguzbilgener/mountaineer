#define SEP '\n' // |
#define SOSPIN 6
#define ALTITUDE 1005.0


#include <SoftwareSerial.h>
#include <SFE_BMP180.h>
#include <Wire.h>

SoftwareSerial bt(7,8);

char sosMessage[8];
char sensorMessage[17];

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
//  bt.begin(9600);
  pinMode(SOSPIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  lastSos = LOW;
  sos = LOW;

  Serial.println("setup()");

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

  if (pressure.begin()) {
       Serial.println("BMP180 init success"); 
  }
  else
  {
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
  
}

void loop() {  
  
  sos = digitalRead(SOSPIN);
  
  readBluetooth();  
  readHardware();

  readPressure();
   

   if (sos == HIGH && lastSos != HIGH) {
      sendSosSignal();
      lastSos = sos;
      sos = LOW;
   }
   else {
      lastSos = sos;
   }

   if(gotBtMessage) {
//      Serial.print(inBtBuffer);     
      clearBtBuffer();
   }
   
   if(gotHwMessage){      
//      bt.print(inBuffer2);
      clearHwBuffer();
 
   }
}

void sendSensorValues(){
  sensorMessage[0] = '$';
  sensorMessage[1] = '$';
  sensorMessage[2] = '$';
  sensorMessage[3] = '3';
  sensorMessage[3] = '2';
  sensorMessage[4] = tempArr[0];
  sensorMessage[5] = tempArr[1];
  sensorMessage[6] = '.';
  sensorMessage[7] = tempArr[2];
  sensorMessage[8] = '#';
  sensorMessage[9] = altArr[0];
  sensorMessage[10] = altArr[1];
  sensorMessage[11] = altArr[2];
  sensorMessage[12] = altArr[3];
  sensorMessage[13] = '$';
  sensorMessage[14] = '$';
  sensorMessage[15] = '$';
  sensorMessage[16] = '\0';
  for (int i=0;i<16;i++) {
    Serial.print(sensorMessage[i]);
  }
  Serial.print(SEP);
  
//  Serial.println(sensorMessage);
//  Serial.println(altArr);
  delay(1000);
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

  Serial.print(sosMessage);
  Serial.print(SEP);
//  Serial.println(sosMessage);
//  bt.print(sosMessage);
  
}

void readPressure() {
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    
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
          
            char alt1char = char (alt1);
            char alt2char = char (alt2);
            char alt3char = char (alt3);
            char alt4char = char (alt4);
          
            altArr[0] = alt1char;
            altArr[1] = alt2char;
            altArr[2] = alt3char;
            altArr[3] = alt4char;
            
            
            // Temp Char Calculations
            int tempValue = analogRead(A0);
            float temp = tempValue * 0.48828125;
            int tempInt = temp;
            int temp1 = temp / 10;
            int temp2 = temp - (temp1 * 10);
            int temp3 = (temp  * 10) - (tempInt * 10);
          
            char temp1char = char(temp1);
            char temp2char = char(temp2);
            char temp3char = char(temp3);
          
            tempArr[0] = temp1char;
            tempArr[1] = temp2char;
            tempArr[2] = temp3char;

            Serial.print(P);
            Serial.print(" ");
            Serial.print(p0);
            Serial.print(" ");
            Serial.println(T);
            delay(400);

//            Serial.print(temp);
//            Serial.print("----");
//            Serial.println(tempValue);
//            delay(500);
            
//            sendSensorValues();
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
      inBtBuffer[btPos] = inBluetooth;
      btPos ++;
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
      inHwBuffer[hwPos] = inHw;
      hwPos ++;      
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

