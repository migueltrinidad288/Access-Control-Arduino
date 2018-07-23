#include <Keypad.h>

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

byte rowPinsOut[ROWSIN] = { 13, 12, 11, 10 };    
byte colPinsOut[COLSIN] = { A0, A1, A2, A3 }; 

Keypad kpd = Keypad( makeKeymap(keysin), rowPinsIn, colPinsIn, ROWSIN, COLSIN ); 
Keypad keypad = Keypad( makeKeymap(keysout), rowPinsOut, colPinsOut, ROWSOUT, COLSOUT ); 

void setup()
{                             
  Serial.begin(9600);                     
}

void loop()
{
   In = GetNumberIn();
   Serial.println ();
   Serial.print("In: ");
   Serial.print(In);
   Serial.println ();
   Out = GetNumberOut();
   Serial.println ();
   Serial.print("Out: ");
   Serial.print(Out);
   Serial.println ();
   In = "";
   Out = "";

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
            Serial.print(keyin);
            numin = numin + keyin;
            //num = num * 10 + (key - '0');
            break;

         case '*':
            numin = "";
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
            Serial.print(keyout);
            numout = numout + keyout;
            //num = num * 10 + (key - '0');
            break;

         case '*':
            numout = "";
            break;
      }

      keyout = keypad.getKey();
   }
   return numout;
}
