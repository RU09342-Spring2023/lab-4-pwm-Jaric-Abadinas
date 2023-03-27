# README file:
.c file: Part1.c

## Task:
Generate a 1 kHz PWM signal with a duty cyle between 0% and 100%. When processor starts up, both red and green LEDs should be at 50% duty cycle. Upon pressing a button, the duty cycle of that respective LED should increase by 10%. Once you have reached 100%, duty cycle should go back down to 0%.

## Prototypes and Main Function:
This section consisted of the prototypes used along with the main function of the code.
{
#include <msp430.h>


void LEDSetup();
void ButtonSetup();
void TimerB0Setup();
void TimerB1Setup();


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // stop watchdog timer

    LEDSetup();     // Initialize our LEDS
    ButtonSetup();  // Initialize our button
    TimerB0Setup(); // Initialize Timer0
    TimerB1Setup(); // Initialize Timer1

    PM5CTL0 &= ~LOCKLPM5;                     // turn on GPIO
    __bis_SR_register(LPM3_bits | GIE);       // Enter LPM3 w/ interrupt
    __no_operation();
}
}
