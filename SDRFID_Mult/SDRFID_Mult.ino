#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>

File logFile;
File myFile;

String datatemp;
String dataNames[160] = {};     //SD csv to array, column1
String dataTagIDs[160] = {};     //SD csv to array, column2
int rowcount = 0; //rows in dataNames & dataTagIDs

constexpr uint8_t RST_PIN = 8;     // Configurable, see typical pin layout above
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
DS3231  rtc(20, 21);

void setup() {
  // put your setup code here, to run once:
  
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
  rtc.begin();
  
  if (SD.begin(4))
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  loadNames();

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

void loadNames() {
  String datatemp;          //temp to store datapoint
  myFile = SD.open("users.csv");
  if (myFile) {
    Serial.println("Read ""users.csv" ":");
    lcd.clear();
    lcd.print("File Read");
    lcd1.clear();
    lcd1.print("File Read");
    delay(1000);
    // Reading the whole file
    while (myFile.available()) {
      char filedata = myFile.read();

      if ((filedata != ',') && (filedata != '\n')  && (filedata != '\r')) {
        datatemp = datatemp += filedata;
      }

      if (filedata == ',') {
        Serial.print(" Name (" + String(rowcount) + "): " + datatemp + "");
        dataNames[rowcount] = datatemp;
        datatemp = "";
      }
      if (filedata == '\n') {
        dataTagIDs[rowcount] = datatemp;
        Serial.println(", TagID (" + String(rowcount) + "): " + datatemp + "" );
        rowcount++;
        datatemp = "";
      }
    }
    myFile.close();
    Serial.println(" Done with reading.");
  }
  else {
    Serial.println("Error opening ""users.csv");
    lcd.clear();
    lcd.print("Error");
    lcd1.clear();
    lcd1.print("Error");    
  }
}

void tag() {
  // Look for new cards
  //lcd.clear();
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
  
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
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
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    content0.concat(String(buffer[i] < 0x10 ? "0" : ""));
    content0.concat(String(buffer[i], HEX));
  }
  Serial.println();
  content0.toUpperCase();
  Read0 = content0;
  Named();
}

void dump_byte_array1(byte *buffer, byte bufferSize) {
  String content1 = "";
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    content1.concat(String(buffer[i] < 0x10 ? "0" : ""));
    content1.concat(String(buffer[i], HEX));
  }
  Serial.println();
  content1.toUpperCase();
  Read1 = content1;
  Names();
}

void Named(){
  Name = "";
  for (int i = 0; i <= rowcount; i++) {
    if (Read0.equals(dataTagIDs[i])) {
      Name = dataNames[i];
      i = rowcount + 1;
    }
  }
  if (Name != ""){
    String datalogstring = "";
    datalogstring = String(Read0) + "," + String(Name) + "," + "Log IN" + "," + String(rtc.getDateStr()) + "," + String(rtc.getTimeStr()) + "," + "SUCCESS";
    File dataFile = SD.open("data.csv", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(datalogstring);
      dataFile.close();
      // print to the serial port too:
      Serial.println(datalogstring);
    }
  else {
    Serial.println("error opening datalog.csv");
  }
    match();
  }
  else {
    String datalogstring = "";
    datalogstring = String(Read0) + "," + String(Name) + "," + "Log IN" + "," + String(rtc.getDateStr()) + "," + String(rtc.getTimeStr()) + "," + "DENIED";
    File dataFile = SD.open("data.csv", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(datalogstring);
      dataFile.close();
      // print to the serial port too:
      Serial.println(datalogstring);
    }
    else {
      Serial.println("error opening datalog.csv");
    }
    denied();
  }
  return "N/A";
}

void Names(){
  Name = "";
  for (int i = 0; i <= rowcount; i++) {
    if (Read1.equals(dataTagIDs[i])) {
      Name = dataNames[i];
      i = rowcount + 1;
    }
  }
  if (Name != ""){
    String datalogstring = "";
    datalogstring = String(Read1) + "," + String(Name) + "," + "Log OUT" + "," + String(rtc.getDateStr()) + "," + String(rtc.getTimeStr()) + "," + "SUCCESS";
    File dataFile = SD.open("data.csv", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(datalogstring);
      dataFile.close();
      // print to the serial port too:
      Serial.println(datalogstring);
    }
    else {
      Serial.println("error opening datalog.csv");
    }
    match1();
  }
  else {
    String datalogstring = "";
    datalogstring = String(Read1) + "," + String(Name) + "," + "Log OUT" + "," + String(rtc.getDateStr()) + "," + String(rtc.getTimeStr()) + "," + "DENIED";
    File dataFile = SD.open("data.csv", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(datalogstring);
      dataFile.close();
      // print to the serial port too:
      Serial.println(datalogstring);
    }
    else {
      Serial.println("error opening datalog.csv");
    }
    denied1();
  }
  return "N/A";
}

void loop() {
  // put your main code here, to run repeatedly:
  tag();
}

void match() {
  Serial.println("Match");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Name);
  lcd.setCursor(0,1);
  lcd.print("Match!");
  Serial.println(Name);
  Serial.print(rtc.getDateStr());
  Serial.print(" - ");
  Serial.print(rtc.getTimeStr());
  Serial.print(" -- ");
  Serial.println(rtc.getDOWStr());
  Serial.println();
  digitalWrite(relay, LOW);
  digitalWrite(red, -250);
  digitalWrite(green, 0);
  digitalWrite(blue, -250);
  tone(buzz, 100000,1000);
  delay(2000);
  noTone(buzz);
  digitalWrite(relay, HIGH);
  Read0 = "";
  lcd.clear();
  tag();
}

void denied() {
  Serial.println("DENIED");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Read0);
  lcd.setCursor(0,1);
  lcd.print("DENIED");
  Serial.print(rtc.getDateStr());
  Serial.print(" - ");
  Serial.print(rtc.getTimeStr());
  Serial.print(" -- ");
  Serial.println(rtc.getDOWStr());
  Serial.println();
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
  Serial.println("Match");
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print(Name);
  lcd1.setCursor(0,1);
  lcd1.print("Match!");
  Serial.println(Name);
  Serial.print(rtc.getDateStr());
  Serial.print(" - ");
  Serial.print(rtc.getTimeStr());
  Serial.print(" -- ");
  Serial.println(rtc.getDOWStr());
  Serial.println();
  digitalWrite(relay, LOW);
  digitalWrite(red1, -250);
  digitalWrite(green1, 0);
  digitalWrite(blue1, -250);
  tone(buzz, 100000,1000);
  delay(2000);
  noTone(buzz);
  digitalWrite(relay, HIGH);
  Read1 = "";
  lcd1.clear();
  tag();
}

void denied1() {
  Serial.println("DENIED");
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print(Read1);
  lcd1.setCursor(0,1);
  lcd1.print("DENIED");
  Serial.print(rtc.getDateStr());
  Serial.print(" - ");
  Serial.print(rtc.getTimeStr());
  Serial.print(" -- ");
  Serial.println(rtc.getDOWStr());
  Serial.println();
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

