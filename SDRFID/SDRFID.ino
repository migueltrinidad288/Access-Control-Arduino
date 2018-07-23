#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

File logFile;
File myFile;

String datatemp;
String dataNames[160] = {};     //SD csv to array, column1
String dataTagIDs[160] = {};     //SD csv to array, column2
int rowcount = 0; //rows in dataNames & dataTagIDs

#define SS_PIN 53
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);
String ReadCon = "gdg";
char readCard[4];

String Name;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int relay = 41;
int buzz = 46;

int red = A8;
int green = A9;
int blue = A10;

void setup() {
  // put your setup code here, to run once:
  pinMode(buzz, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  noTone(buzz);
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

//Scans a RFID Card
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
  Named();
}

//Find RFID's UID from the SD Card Database
void Named() {
  Name = "";
  for (int i = 0; i <= rowcount; i++) {
    if (ReadCon.equals(dataTagIDs[i])) {
      Name = dataNames[i];
      i = rowcount + 1;
    }
  }
  if (Name != "") {
    match();
  }
  else {
    denied();
  }
  return "N/A";
}


void loop() {
  tag();
}

//Function if the RFID's UID is inside the SD Card Database
void match() {
  Serial.println("Match");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Name);
  lcd.setCursor(0, 1);
  lcd.print("Match!");
  Serial.println(Name);
  digitalWrite(relay, LOW);
  digitalWrite(red, -250);
  digitalWrite(green, 0);
  digitalWrite(blue, -250);
  tone(buzz, 100000, 1500);
  delay(2000);
  digitalWrite(relay, HIGH);
  ReadCon = "gdg";
  noTone(buzz);
  lcd.clear();
  tag();
}

//Function if the RFID's UID is not inside the SD Card Database
void denied() {
  Serial.println("DENIED");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(ReadCon);
  lcd.setCursor(0, 1);
  lcd.print("DENIED");
  digitalWrite(red, 0);
  digitalWrite(green, -250);
  digitalWrite(blue, -250);
  digitalWrite(relay, HIGH);
  tone(buzz, 60000);
  delay(1000);
  lcd.clear();
  noTone(buzz);
  tag();
}

