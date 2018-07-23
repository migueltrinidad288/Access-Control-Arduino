#include <SD.h>
#include <SPI.h>

File printFile;
String buffer;
boolean SDfound;
String req;

void setup() {
  Serial.begin(9600);

  if (SDfound == 0) {
    if (!SD.begin(53)) {
      Serial.print("The SD card cannot be found");
      while(1);
    }
  }
  SDfound = 1;
  printFile = SD.open("hey.txt");

  if (!printFile) {
    Serial.print("The text file cannot be opened");
    while(1);
  }

  while (printFile.available()) {
    req = printFile.readUntil('avfd45l3');
    Serial.println(req); //Printing for debugging purpose         
    //do some action here
  }

  printFile.close();
}

void loop() {
   //empty
}
