#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h> 
#include <SD.h>
#include <RTClib.h>

SoftwareSerial SerialBT(6, 7);  //RX, TX
LiquidCrystal_I2C lcd(0x27, 16, 2);

int tombol = 4;
int relay = 2;
int tgssensor = A1;
int mq135 = A0;
int analogTGS;
int analogMQ;
String strTGSValue, strMQValue;
const int chipSelect = 5;
RTC_DS1307 rtc;

void setup() {
  pinMode(tombol, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(tgssensor, INPUT);
  pinMode(mq135, INPUT);
  Serial.begin(9600);
  SerialBT.begin(9600);

  while (!Serial);
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }
    else {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    if (! rtc.isrunning()) {
      Serial.println("RTC is NOT running!");
    }
    
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  //LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("WELCOME");
  lcd.setCursor(1, 1);
  lcd.print("PUSH SAMPLING");
}

void loop() {
  if (digitalRead(tombol) == LOW) {
    for (int k = 1; k < 81; k++) {
      //pump
      digitalWrite(relay, HIGH);

      //analog sensor value
      analogTGS = analogRead(tgssensor);
      analogMQ = analogRead(mq135);

      //ppm consentration
      float voltTGS = (analogTGS/1024.0)*5.0;
      float rsTGS = ((5.0/voltTGS)-1)*1.01;
      float rasioTGS = rsTGS/21.3631;
      float ppmTGS = (exp((rasioTGS-0.2746)/-0.076));
      
      float voltMQ = (analogMQ/1024.0)*5.0;
      float rsMQ = ((5.0/voltMQ)-1)*2.40;
      float rasioMQ = rsMQ/3.7569;
      float ppmMQ = (exp((rasioMQ-0.0255)/-0.777));
      
      //serial monitor/arduspreadsheet
      Serial.print(analogTGS);
      Serial.print("\t");
      Serial.print(ppmTGS,3);
      Serial.print("\t");
      Serial.print(analogMQ);
      Serial.print("\t");
      Serial.print(ppmMQ,3);
      Serial.print("\n");

      //lcd
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Toluena:");
      lcd.setCursor(9, 0);
      lcd.print(ppmTGS,3);
      lcd.setCursor(0, 1);
      lcd.print("Amonia:");
      lcd.setCursor(9, 1);
      lcd.print(ppmMQ,3);

      //bluetooth master to slave
      strTGSValue = String(ppmTGS,3);
      strMQValue = String(ppmMQ,3);
      SerialBT.println('*'+strTGSValue+','+strMQValue+'#');

      delay(1000);
    }
    lcd.clear();
    lcd.setCursor(8,0);
    lcd.print("DONE");
    lcd.setCursor(4,1);
    lcd.print("SAMPLING");
  } else {
    //pump
    digitalWrite(relay, LOW);

  }
}
