#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial SerialBT(10,11); //RX, TX
LiquidCrystal_I2C lcd(0x27, 16, 2);

String dataIn;
String dt[10];
int i;
boolean parsing=false;

void setup() {
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  SerialBT.begin(9600);
  lcd.setCursor(5,0);
  lcd.print("WELCOME");
  lcd.setCursor(2,1);
  lcd.print("WAITING DATA");
  dataIn="";
}
void loop() {
  if(SerialBT.available()>0){
    char inChar = (char)SerialBT.read();
    dataIn += inChar;
    if (inChar == '\n'){
      parsing = true;
    }
  }
  if (parsing){
    parsingData(); //memanggil fungsi parsing
    parsing=false;
    dataIn="";
  }
}

void parsingData(){
int j=0;

//inisialisasi variabel
dt[j]="";
//proses parsing data
for(i=1;i<dataIn.length();i++){
  
//pengecekan tiap karakter dengan karakter (#) dan (,)
if ((dataIn[i] == '#') || (dataIn[i] == ',')){
j++; 
dt[j]="";  //inisialisasi variabel array dt[j]
}
else{
//proses tampung data saat pengecekan karakter selesai.
dt[j] = dt[j] + dataIn[i];
}
}
  SerialBT.flush();
  lcd.flush();

  //LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Toluena:");
  lcd.setCursor(9, 0);
  lcd.print(dt[0]);
  lcd.setCursor(0, 1);
  lcd.print("Amonia:");
  lcd.setCursor(9, 1);
  lcd.print(dt[1]);
  Serial.print(dt[0]);
  //Serial.print("\t");
  Serial.print(",");
  Serial.print(dt[1]);
  Serial.println(",");
}
