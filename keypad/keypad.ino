#include <Keypad.h>

String v1 = "";
String v2 = "";
int v3 = 0;
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {                    
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = { 9, 8, 7, 6 };    
byte colPins[COLS] = { 5, 4, 3, 2 }; 
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); 

void setup()
{                             
  Serial.begin(9600);                     
}

void loop()
{
   v1 = GetNumber();
   Serial.println ();
   Serial.print("v1: ");
   Serial.print(v1);
   Serial.println ();
   v2 = "";
   v1 = "";

}

String GetNumber()
{
   String num = "";
   char key = kpd.getKey();
   while(key != 'A')
   {
      switch (key)
      {
         case NO_KEY:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            Serial.print(key);
            num = num + key;
            //num = num * 10 + (key - '0');
            break;

         case '*':
            num = "";
            break;
      }

      key = kpd.getKey();
   }
   return num;
}
