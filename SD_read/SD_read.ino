#include <SD.h>
#include <SPI.h>

File printFile;
String buffer;
boolean SDfound;


void setup() {
  Serial.begin(9600);

  if (SDfound == 0) {
    if (!SD.begin(4)) {
      Serial.println("The SD card cannot be found");
      while (1);
    }
  }
  SDfound = 1;
  printFile = SD.open("hey.txt");

  if (!printFile) {
    Serial.print("The text file cannot be opened");
    while (1);
  }

  while (printFile.available()) {
    buffer = printFile.readStringUntil('\n');
    Serial.println(buffer); //Printing for debugging purpose
    //do some action here
  }

  printFile.close();
}

void loop() {
  //empty
}
