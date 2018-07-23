#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

constexpr uint8_t RST_PIN = A7;     // Configurable, see typical pin layout above
constexpr uint8_t SS_1_PIN = 53;   // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
constexpr uint8_t SS_2_PIN = 49;    // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1
constexpr uint8_t NR_OF_READERS = 2;
byte ssPins[] = {SS_1_PIN, SS_2_PIN};
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

String Read0 = "";
String Read1 = "";

String Name;
LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd1(0x26, 16, 2);

int relay = 41;

int red = A8;
int green = A9;
int blue = A10;

int red1 = A11;
int green1 = A12;
int blue1 = A13;

int buzz = 46;

String RPI = "";

String In = "";
String Out = "";
const byte ROWSIN = 4;
const byte COLSIN = 4;
const byte ROWSOUT = 4;
const byte COLSOUT = 4;

char keysin[ROWSIN][COLSIN] = {                    
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char keysout[ROWSOUT][COLSOUT] = {                    
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPinsIn[ROWSIN] = { 9, 8, 7, 6 };  
byte colPinsIn[COLSIN] = { 5, 4, 3, 2 }; 

byte rowPinsOut[ROWSIN] = { 22, 24, 26, 28 };    
byte colPinsOut[COLSIN] = { 30, 32, 34, 36 }; 

Keypad kpd = Keypad( makeKeymap(keysin), rowPinsIn, colPinsIn, ROWSIN, COLSIN ); 
Keypad keypad = Keypad( makeKeymap(keysout), rowPinsOut, colPinsOut, ROWSOUT, COLSOUT );

boolean a = true;
boolean b = true;
int pb = 10;
void setup() {
  // put your setup code here, to run once:
  pinMode(pb, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(buzz, OUTPUT);
  noTone(buzz);
  
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  digitalWrite(relay, HIGH);
  digitalWrite(red, -250);
  digitalWrite(green, -250);
  digitalWrite(blue, 0);
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  
  pinMode(red1, OUTPUT);
  pinMode(green1, OUTPUT);
  pinMode(blue1, OUTPUT);
  digitalWrite(red1, -250);
  digitalWrite(green1, -250);
  digitalWrite(blue1, 0);
  lcd1.begin(16,2);
  lcd1.init();
  lcd1.backlight();
  
  Serial.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready To Scan!");

  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Ready To Scan!");
  
  tag();
}

void key0(){
  lcd.clear();
  In = GetNumberIn();
  Serial.println(In + "IN");
}

void key1(){
  lcd1.clear();
  Out = GetNumberOut();
  Serial.println(Out + "OU");
}

void tag() {
  // Look for new cards
  //lcd.clear();
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards
    lcd.setCursor(0, 0);
    lcd.print("Ready To Scan!");
    digitalWrite(red, -250);
    digitalWrite(green, -250);
    digitalWrite(blue, 0);
  
    lcd1.setCursor(0, 0);
    lcd1.print("Ready To Scan!");
    digitalWrite(red1, -250);
    digitalWrite(green1, -250);
    digitalWrite(blue1, 0);

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      if (reader == 0){
      dump_byte_array0(mfrc522[0].uid.uidByte, mfrc522[0].uid.size);
      }
      else if (reader == 1){
      dump_byte_array1(mfrc522[1].uid.uidByte, mfrc522[1].uid.size);
      }
    }
  }
}

void dump_byte_array0(byte *buffer, byte bufferSize) {
  String content0 = "";
  for (byte i = 0; i < bufferSize; i++) {
    content0.concat(String(buffer[i] < 0x10 ? "0" : ""));
    content0.concat(String(buffer[i], HEX));
  }
  content0.toUpperCase();
  Read0 = content0;
  Serial.println(Read0 + "IN");
}

void dump_byte_array1(byte *buffer, byte bufferSize) {
  String content1 = "";
  for (byte i = 0; i < bufferSize; i++) {
    //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    //Serial.print(buffer[i], HEX);
    content1.concat(String(buffer[i] < 0x10 ? "0" : ""));
    content1.concat(String(buffer[i], HEX));
  }
  content1.toUpperCase();
  Read1 = content1;
  Serial.println(Read1 + "OU");
}

void loop() {
  // put your main code here, to run repeatedly:
  a = true;
  b = true;
  char keyi = kpd.getKey();
  char keyo = keypad.getKey();
  if (keyi == '*' ){
    a = false;
    lcd.clear();
    //Serial.print(a);
  }
  if (keyo == '*'){
    b = false;
    lcd1.clear();
    //Serial.print(b);
  }
  if ((a == true) || (b == true)){
    //Serial.print(a);
    //Serial.print(b);
    tag();
  }
  if (a == false){
    //Serial.print(a);
    //Serial.print(a);
    key0();
  }
  if (b == false){
    // Serial.print(a);
    key1();
  }
  
  delay(50);
  if (Serial.available()){
    RPI = Serial.readString();
  }
  if (RPI == "SUCCESSIN"){
    RPI = "";
    match();
  }
  else if (RPI == "SUCCESSOUT"){
    RPI = "";
    match1();
  }
  else if (RPI == "DENIEDIN"){
    RPI = "";
    denied();
  }
  else if (RPI == "DENIEDOUT"){
    RPI = "";
    denied1();
  }
}

String GetNumberIn()
{
   String numin = "";
   char keyin = kpd.getKey();
   while(keyin != 'A')
   {
      switch (keyin)
      {
         case NO_KEY:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            //Serial.print(keyin);
            lcd.print("*");
            numin = numin + keyin;
            //num = num * 10 + (key - '0');
            break;

         case 'A':
            numin = "";
            break;
         case 'C':
            numin = "";
            lcd.clear();
            break;
      }

      keyin = kpd.getKey();
   }
   return numin;
}

String GetNumberOut()
{
   String numout = "";
   char keyout = keypad.getKey();
   while(keyout != 'A')
   {
      switch (keyout)
      {
         case NO_KEY:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            //Serial.print(keyout);
            lcd1.print("*");
            numout = numout + keyout;
            //num = num * 10 + (key - '0');
            break;

         case 'A':
            numout = "";
            break;
         case 'C':
            numout = "";
            lcd1.clear();
            break;
      }

      keyout = keypad.getKey();
   }
   return numout;
}

void match() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Match!");
  digitalWrite(relay, LOW);
  digitalWrite(red, -250);
  digitalWrite(green, 0);
  digitalWrite(blue, -250);
  tone(buzz, 100000,1000);
  delay(2000);
  noTone(buzz);
  digitalWrite(relay, HIGH);
  In = "";
  Read0 = "";
  lcd.clear();
  tag();
}

void denied() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("DENIED");
  digitalWrite(red, 0);
  digitalWrite(green, -250);
  digitalWrite(blue, -250);
  digitalWrite(relay, HIGH);
  tone(buzz, 600000);
  delay(1000);
  noTone(buzz);
  lcd.clear();
  tag();
}

void match1() {
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print("Match!");
  digitalWrite(relay, LOW);
  digitalWrite(red1, -250);
  digitalWrite(green1, 0);
  digitalWrite(blue1, -250);
  tone(buzz, 100000,1000);
  delay(2000);
  noTone(buzz);
  digitalWrite(relay, HIGH);
  Out = "";
  Read1 = "";
  lcd1.clear();
  tag();
}

void denied1() {
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print("DENIED");
  digitalWrite(red1, 0);
  digitalWrite(green1, -250);
  digitalWrite(blue1, -250);
  digitalWrite(relay, HIGH);
  tone(buzz, 600000);
  delay(1000);
  noTone(buzz);
  lcd1.clear();
  tag();
}

