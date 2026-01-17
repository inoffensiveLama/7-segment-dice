#include <Arduino.h>

// pin numbers for 7 segment displays (two in serial with shift registers)
const int seven_seg_latch_pin = 3;
const int seven_seg_clock_pin = 4;
const int seven_seg_data_pin = 2;

// pin numbers for dip switch shift register
const int dip_clock_pin = 8;
const int dip_latch_pin = 9;
const int dip_data_pin = 10;

// pin number for button to "throw dice"
const int Throw_dice_pin = 5;

unsigned long last_button_press = 0;

// array that holds all the binary values for the shiftregisters
int digits[10][8]{
    {0, 0, 1, 0, 0, 0, 0, 1}, // digit 0
    {1, 0, 1, 0, 1, 1, 1, 1}, // digit 1
    {0, 0, 1, 1, 0, 0, 1, 0}, // digit 2
    {0, 0, 1, 0, 0, 1, 1, 0}, // digit 3
    {1, 0, 1, 0, 1, 1, 0, 0}, // digit 4
    {0, 1, 1, 0, 0, 1, 0, 0}, // digit 5
    {0, 1, 1, 0, 0, 0, 0, 0}, // digit 6
    {0, 0, 1, 0, 1, 1, 1, 1}, // digit 7
    {0, 0, 1, 0, 0, 0, 0, 0}, // digit 8
    {0, 0, 1, 0, 0, 1, 0, 0}  // digit 9

    /*
    (A, B, DP, C, D, E, F, G)

      * A *
      F   B
      * G *
      E   C
      * D * DP
    */

};

/*

order of dipswitches:

number switch - digit in dip_switch_value
1 - 4
2 - 5
3 - 6
4 - 7
5 - 3
6 - 2
7 - 1
8 - 0

*/

// put function declarations here:
void WriteDigitToShiftRegister(int Digit);
uint8_t readDipSwitch();
void WriteErrorToShiftRegister();

void setup()
{
    pinMode(dip_latch_pin, OUTPUT);
    pinMode(dip_clock_pin, OUTPUT);
    pinMode(dip_data_pin, INPUT);

    pinMode(seven_seg_latch_pin, OUTPUT);
    pinMode(seven_seg_clock_pin, OUTPUT);
    pinMode(seven_seg_data_pin, OUTPUT);

    pinMode(Throw_dice_pin, INPUT_PULLUP);

    digitalWrite(dip_latch_pin, HIGH);
    digitalWrite(dip_clock_pin, LOW);

}

void loop()
{
    // // put your main code here, to run repeatedly:

    if (digitalRead(Throw_dice_pin) == LOW)
    {
        // this is here so the function is not executed multiple times while only pushing the button once
        if (last_button_press + 300 <= millis())
        {
            //these will be used for the throw
            int dice_ceiling = 0;
            int dice_bottom = 0;
            int number_of_dice = 1;
            int dice_throw = 0;

            //check which "dice" to throw
            uint8_t dip_switch_Value = readDipSwitch();

            switch (dip_switch_Value)
            {
            //2 sided dice
            case 0b00001000:
                dice_ceiling = 2;
                dice_bottom = 1;
                break;
            
            //2 * 2 sided dice
            case 0b10001000:
                dice_ceiling = 2;
                dice_bottom = 1;
                number_of_dice = 2;
                break;
            
            //4 sided dice
            case 0b00000100:
                dice_ceiling = 4;
                dice_bottom = 1;
                break;

            //2 * 4 sided dice
            case 0b10000100:
                dice_ceiling = 4;
                dice_bottom = 1;
                number_of_dice = 2;
                break;
            
            //6 sided dice
            case 0b00000010:
                dice_ceiling = 6;
                dice_bottom = 1;
                break;

            //2 * 6 sided dice
            case 0b10000010:
                dice_ceiling = 6;
                dice_bottom = 1;
                number_of_dice = 2;
                break;

            //8 sided dice
            case 0b00000001:
                dice_ceiling = 8;
                dice_bottom = 1;
                break;

            //2 * 8 sided dice
            case 0b10000001:
                dice_ceiling = 8;
                dice_bottom = 1;
                number_of_dice = 2;
                break;

            //10 sided dice
            case 0b00010000:
                dice_ceiling = 10;
                dice_bottom = 1;
                break;

            //2 * 10 sided dice
            case 0b10010000:
                dice_ceiling = 100;
                dice_bottom = 0;
                break;

            //12 sided dice
            case 0b00100000:
                dice_ceiling = 12;
                dice_bottom = 1;
                break;

            //20 sided dice
            case 0b01000000:
                dice_ceiling = 20;
                dice_bottom = 1;
                break;

            //100 sided dice
            case 0b10000000:
                dice_ceiling = 100;
                dice_bottom = 0;
                break;

            default:
                break;
            }

            if(number_of_dice == 1 && dice_ceiling != 0){
                dice_throw = random(dice_bottom, dice_ceiling + 1);
                if (dice_throw < 10)
                {
                    WriteDigitToShiftRegister(dice_throw);
                    WriteDigitToShiftRegister(0);
                }
                else
                {
                    WriteDigitToShiftRegister(dice_throw % 10);
                    WriteDigitToShiftRegister(dice_throw / 10);
                }
            } else if(number_of_dice == 2 && dice_ceiling != 0){
                dice_throw = random(dice_bottom, dice_ceiling + 1);
                WriteDigitToShiftRegister(dice_throw);
                dice_throw = random(dice_bottom, dice_ceiling + 1);
                WriteDigitToShiftRegister(dice_throw);
            } else {
                WriteErrorToShiftRegister();
            }
            

            last_button_press = millis();
        }
    }

}

uint8_t readDipSwitch()
{
    // resets value to 00000000
    uint8_t dip_Switch_Value = 0;

    // setting this pin to HIGH prevents any change of the state in the SN74HC165 shift register
    digitalWrite(dip_latch_pin, HIGH);

    for (int i = 0; i < 8; i++)
    {
        // moves all values to the left once (ie 00010000 -> 00100000)
        dip_Switch_Value <<= 1;
        // puts value of pin in the variable with bitwise OR operator
        dip_Switch_Value |= !digitalRead(dip_data_pin);

        // moves all values in the SN74HC165 shift register by one
        digitalWrite(dip_clock_pin, HIGH);
        delayMicroseconds(5);
        digitalWrite(dip_clock_pin, LOW);
        delayMicroseconds(5);
    }

    digitalWrite(dip_latch_pin, LOW);
    return dip_Switch_Value;
}


// function to send a 1 digit number to the shift register (if you have more than 1 digit simply call it as many times as needed, but from back to front)
void WriteDigitToShiftRegister(int Digit)
{
    // they need to be sent in to the shift register the "wrong way" so the last digit is being sent in first
    for (int i = 7; i >= 0; i--)
    {
        digitalWrite(seven_seg_latch_pin, LOW);
        digitalWrite(seven_seg_clock_pin, LOW);
        if (digits[Digit][i] == 1)
        {
            digitalWrite(seven_seg_data_pin, LOW);
        }
        if (digits[Digit][i] == 0)
        {
            digitalWrite(seven_seg_data_pin, HIGH);
        }
        digitalWrite(seven_seg_clock_pin, HIGH);
        digitalWrite(seven_seg_latch_pin, HIGH);
        // delay(30);
    }
}


// function to send a letters Er to the shift register
void WriteErrorToShiftRegister()
{
    //g,f,e,d,c,dp,b,a,g,f,e,d,c,dp,b,a
    //0,1,0,1,1,1 ,1,1,0,0,0,0,1,1 ,1,0
    for (int i = 15; i >= 0; i--)
    {
        digitalWrite(seven_seg_latch_pin, LOW);
        digitalWrite(seven_seg_clock_pin, LOW);
        if (i == 15 || i == 13 || i == 7 || i == 6 || i == 5 || i == 4 || i == 0)
        {
            digitalWrite(seven_seg_data_pin, HIGH);
        }
        else
        {
            digitalWrite(seven_seg_data_pin, LOW);
        }
        digitalWrite(seven_seg_clock_pin, HIGH);
        digitalWrite(seven_seg_latch_pin, HIGH);
        // delay(30);
    }
}
