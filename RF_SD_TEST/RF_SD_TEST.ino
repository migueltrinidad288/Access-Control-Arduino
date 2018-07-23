#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

SoftwareSerial rfid = SoftwareSerial(53, 8);
String dataNames[160] = {};     //SD csv to array, column1
String dataTagIDs[160] = {};     //SD csv to array, column2
int rowcount = 1; //rows in dataNames & dataTagIDs

File logFile;
File myFile;

char readCard[4];
#define SS_PIN 53
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);
String ReadCon;

LiquidCrystal_I2C lcd(0x27,16,2);

int relay = 41;

void setup() {
  // put your setup code here, to run once:
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  lcd.init();
  Serial.begin(9600);   // Initiate a serial communication
  while (!Serial) {
   ; // wait for serial port to connect. Needed for Leonardo only
    }


  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(4, OUTPUT);
  
  if (!SD.begin(4)) {
   Serial.println("initialization failed!");
   return;
  }
  Serial.println("initialization done.");  
  loadNames();
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  readTag();
  Serial.println("Put your card to the reader...");
  Serial.println();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Ready To Scan!");
}
void saveLog(String tagID, String name) {
  // Create/Open file
  logFile = SD.open("data.csv", FILE_WRITE);

  if (logFile) {
    String currentTime = String(year()) + "-" + month() + "-" + day() + " " + hour() + ":" + minute();
    String line = tagID + "," + name + "," + currentTime;
    logFile.println(line);
    logFile.close(); // close the file
    Serial.println("Done: " + line);
  }
  else {
    Serial.println("Error opening " "data.csv");
  }
}
void loadNames() {
  String datatemp;          //temp to store datapoint
  myFile = SD.open("users.csv");
  if (myFile) {
    Serial.println("Read ""users.csv"":");
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
    Serial.println(datatemp);
    Serial.println(" Done with reading.");
  }
  else {
    Serial.println("Error opening ""users.csv");
  }
}

String tagID2name( String tagID) {
  for (int i = 1; i <= rowcount; i++) {
    if (tagID.equals(dataTagIDs[i])) {
      return dataNames[i];
    }
  }
  return "N/A";
}

String readTag(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.println("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
     //Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
     readCard[i]= mfrc522.uid.uidByte[i];
  }
  content.toUpperCase();
  ReadCon = content;
  Serial.println(ReadCon);
  lcd.clear();
  lcd.print(ReadCon);
}
void loop() {
  // put your main code here, to run repeatedly:
  String datatemp;
  //readTag();
  delay(100);
  digitalWrite(53,HIGH);
  digitalWrite(4, LOW);
  //loadNames();
  //Serial.println(datatemp);
  delay(100);

  String tagID = readTag();
  if (tagID != "") {
    
    
    String name = tagID2name(tagID);
    saveLog(tagID, name);

    name.replace(" ", "\n");
    name.replace("á", "a");
    name.replace("é", "e");
    name.replace("í", "i");
    name.replace("ó", "o");
    name.replace("ö", "o");
    name.replace("ő", "o");
    name.replace("ú", "u");
    name.replace("ű", "u");
    name.replace("ü", "u");
    delay(400);
  }

  if(tagID ==ReadCon){
    Serial.print("Match!");
  }
}
