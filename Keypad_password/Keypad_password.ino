
#include <Password.h>

#include <Keypad.h>

Password password = Password( "1234" );

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
 {'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 2, 3, 4, 5 };// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 6, 7, 8, 9 };


// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define ledPin 13

void setup(){

 digitalWrite(ledPin, LOW);   // sets the LED on
 Serial.begin(9600);
 keypad.addEventListener(keypadEvent); //add an event listener for this keypad
 keypad.setDebounceTime(250);
}

void loop(){
 keypad.getKey();
}

//take care of some special events
void keypadEvent(KeypadEvent eKey){
 switch (keypad.getState()){
   case PRESSED:
     Serial.print("Pressed: ");
     Serial.println(eKey);
     switch (eKey){
       case '#': guessPassword(); break;
        default:
              password.append(eKey);
 }
}}

void guessPassword(){
    Serial.print("Guessing password... ");
    if (password.evaluate()){
          digitalWrite(ledPin,HIGH); //activates garaged door relay
            delay(500);                
            digitalWrite(ledPin,LOW); //turns off door relay after .5 sec
          Serial.println("VALID PASSWORD "); //
             password.reset(); //resets password after correct entry
    }else{
          digitalWrite(ledPin,LOW);
          Serial.println("INVALID PASSWORD ");
             password.reset(); //resets password after INCORRECT entry
    }
} 
