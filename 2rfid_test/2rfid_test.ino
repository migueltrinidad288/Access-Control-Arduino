#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);
String ReadCon = "gdg";
char readCard[4];

#define SS_PINb 49
#define RST_PINb 7
MFRC522 mfrc522b(SS_PINb, RST_PINb);
String ReadConb = "gdg";
char readCardb[4];

void setup() {
  Serial.begin(9600);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mfrc522b.PCD_Init();
  
  tag();
  tagb();
}

void tag() {
  // Look for new cards
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
  Serial.println("Message :");
  content.toUpperCase();
  Serial.print("RFID-A: ");
  Serial.println(content);
}

void tagb() {
  // Look for new cards
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
  Serial.println("Message :");
  contentb.toUpperCase();
  Serial.print("RFID-B: ");
  Serial.println(contentb);
}

void loop() {

  // put your main code here, to run repeatedly:
  //Serial.println(dataTagIDs[4]);
  tag();
  tagb();
}

