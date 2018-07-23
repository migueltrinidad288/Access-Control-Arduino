#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>

File myFile;
File printFile;
char buf[10];
String buffer;
char readCard[4];
byte stored[4];
char card[4];
String Read;
String ReadCon;
#define SS_PIN 53
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int relay = 41;
LiquidCrystal_I2C lcd(0x27,16,2);
void setup() 
{
  lcd.init();
  pinMode(relay, OUTPUT);
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
  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Put your card to the reader...");
  Serial.println();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Ready To Scan!");
}
void loop() 
{
  digitalWrite(relay, HIGH);
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
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
     readCard[i]= mfrc522.uid.uidByte[i];
  }
  Serial.println();
  Serial.print("Message :");
  content.toUpperCase();
  Serial.print(content);
  ReadCon = content;
  Serial.println();
  card[4] = readCard[4];
  Serial.println(Read);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(ReadCon);
  digitalWrite(4,LOW);
  digitalWrite(53,HIGH);
}
