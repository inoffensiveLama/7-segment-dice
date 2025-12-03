#include <Arduino.h>

//pin numbers for 7 segment displays (two in serial with shift registers)
const int Latch_pin = 4;
const int Clock_pin = 3;
const int Data_pin = 2;

const int Throw_dice_pin = 20;

//array that holds all the binary values for the shiftregisters
int digits [10][8]{
  {1,1,1,1,1,1,0,0}, // digit 0
  {0,1,1,0,0,0,0,0}, // digit 1
  {1,1,0,1,1,0,1,0}, // digit 2
  {1,1,1,1,0,0,1,0}, // digit 3
  {0,1,1,0,0,1,1,0}, // digit 4
  {1,0,1,1,0,1,1,0}, // digit 5
  {1,0,1,1,1,1,1,0}, // digit 6
  {1,1,1,0,0,0,0,0}, // digit 7
  {1,1,1,1,1,1,1,0}, // digit 8
  {1,1,1,1,0,1,1,0}  // digit 9

  /*
  (A, B, C, D, E, F, G, DP)

    * A *
    F   B
    * G *
    E   C
    * D * DP
  */
};

// put function declarations here:
void WriteDigitToShiftRegister(int Digit);

void setup() {
  // put your setup code here, to run once:

  //setup pinmodes
  pinMode(Latch_pin, OUTPUT);
  pinMode(Clock_pin, OUTPUT);
  pinMode(Data_pin, OUTPUT);
  pinMode(Throw_dice_pin, INPUT_PULLDOWN);

}

void loop() {
  // put your main code here, to run repeatedly:

  if(digitalRead(Throw_dice_pin) == HIGH) {
    int dice_throw = random(20);
    digitalWrite(Latch_pin, LOW);
    if (dice_throw < 10) {
      WriteDigitToShiftRegister(dice_throw);
      WriteDigitToShiftRegister(0);
    } else {
      WriteDigitToShiftRegister(dice_throw % 10);
      WriteDigitToShiftRegister(dice_throw / 10);
    }
    digitalWrite(Latch_pin, HIGH);
  }
}

//function to send a 1 digit number to the shift register (if you have more than 1 digit simply call it as many times as needed, but from back to front)
void WriteDigitToShiftRegister(int Digit)
{
  //they need to be sent in to the shift register the "wrong way" so the last digit is being sent in first 
  for (int i = 7; i>=0; i--)
   {
    digitalWrite(Clock_pin,LOW);
    if (digits[Digit][i]==1) {digitalWrite(Data_pin, LOW); /* Serial.print(0); */} 
    if (digits[Digit][i]==0) {digitalWrite(Data_pin, HIGH); /* Serial.print(1); */}
    digitalWrite(Clock_pin,HIGH);
   }
}