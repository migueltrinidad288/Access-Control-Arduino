#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

File logFile;
File myFile;

String datatemp;
String dataNames[250] = {};     //SD csv to array, column1
String dataTagIDs[250] = {};     //SD csv to array, column2
int rowcount = 0; //rows in dataNames & dataTagIDs

#define SS_PIN 53
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);
String ReadCon = "gdg";
char readCard[4];

#define SS_PINb 50
#define RST_PINb 7
MFRC522 mfrc522b(SS_PINb, RST_PINb);
String ReadConb = "gdg";
char readCardb[4];

String Name;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int relay = 41;

int red = A8;
int green = A9;
int blue = A10;

String List[160] = {};
void setup() {
  // put your setup code here, to run once:
  pinMode(relay, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  digitalWrite(relay, HIGH);
  digitalWrite(red, -250);
  digitalWrite(green, -250);
  digitalWrite(blue, 0);
  lcd.init();
  Serial.begin(9600);

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
  mfrc522.PCD_Init();   // Initiate MFRC522

  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready To Scan!");
  tag();
}

void loadNames() {
  String datatemp;          //temp to store datapoint
  myFile = SD.open("users.csv");
  if (myFile) {
    Serial.println("Read ""users.csv" ":");
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
  }
}

void tag() {
  // Look for new cards
  lcd.setCursor(0, 0);
  lcd.print("Ready To Scan!");
  digitalWrite(red, -250);
  digitalWrite(green, -250);
  digitalWrite(blue, 0);
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  Serial.println();
  Serial.print("Message :");
  content.toUpperCase();
  Serial.println(content);
  ReadCon = content;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(ReadCon);
  Named();
}

void tagb() {
  // Look for new cards
  lcd.setCursor(0, 0);
  lcd.print("Ready To Scan!");
  digitalWrite(red, -250);
  digitalWrite(green, -250);
  digitalWrite(blue, 0);
  if ( ! mfrc522b.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522b.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String contentb = "";
  byte letter;
  for (byte i = 0; i < mfrc522b.uid.size; i++)
  {
    Serial.print(mfrc522b.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(mfrc522b.uid.uidByte[i], HEX);
    contentb.concat(String(mfrc522b.uid.uidByte[i] < 0x10 ? "0" : ""));
    contentb.concat(String(mfrc522b.uid.uidByte[i], HEX));
    readCardb[i] = mfrc522b.uid.uidByte[i];
  }
  Serial.println();
  Serial.print("Message :");
  contentb.toUpperCase();
  Serial.println(contentb);
  ReadConb = contentb;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(ReadConb);
  Names();
}

void Names(){
  for (int i = 0; i <= rowcount; i++) {
    if (ReadConb.equals(dataTagIDs[i])) {
      NamesList();
    }
    else {
      denied();
    }
  }
  return "N/A";
}

void NamesList(){
  for (int i = 0; i <= 160; i++){
    if(ReadConb.equals(List[i])){
      List[i] = "";
      Name = dataNames[i];
      match();
      return dataNames[i];
    }
  }
}

void Named(){
  for (int i = 0; i <= rowcount; i++) {
    if (ReadCon.equals(dataTagIDs[i])) {
      NamedList();
    }
    else {
      denied();
    }
  }
  return "N/A";
}

void NamedList(){
  for (int i = 0; i <= 160; i++){
    if(ReadCon.equals(List[i])){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Already IN");
      denied();
    }
    else{
      Name = dataNames[i];
      List[i] = ReadCon;
      match();
      return dataNames[i];
    }
  }
}

void loop() {

  // put your main code here, to run repeatedly:
  //Serial.println(dataTagIDs[4]);
  tag();
  tagb();
}

void match() {
  Serial.println("Match");
  lcd.clear();
  lcd.print("Match");
  Serial.println(Name);
  digitalWrite(relay, LOW);
  digitalWrite(red, -250);
  digitalWrite(green, 0);
  digitalWrite(blue, -250);
  delay(5000);
  digitalWrite(relay, HIGH);
  ReadCon = "gdg";
  ReadConb = "gdg";
  tag();
  tagb();
}

void denied() {
  Serial.println("DENIED");
  lcd.setCursor(0,1);
  lcd.print("DENIED");
  digitalWrite(red, 0);
  digitalWrite(green, -250);
  digitalWrite(blue, -250);
  digitalWrite(relay, HIGH);
  tag();
  tagb();
}

