# README file:
.c file: Part2.c

## Task:
You will need to use pins 6.0, 6.1, and 6.2 to drive an RGB LED. These will need to be configured with a PWM Period of 1s. You need your RGB LED to cycle between the following colors in order:
- Red
- Orange (Red + Green)
- Green
- Cyan (Green + Blue)
- Blue
- Purple (Red + Blue)
You need to cover colors in between them, meaning as you transition from Red to Orange, it shouldn't be just 2 colors. The amount of colors are up to you, but is needs to appear smooth in transition. The timing for cycling is up to you to determine as well.

## Definitions:
This section consists of definitions of commonly used machine language and turns them to user language to make the code easier to read for other programmers
```c
#define RED_TO_GREEN 0
#define GREEN_TO_BLUE 1
#define BLUE_TO_RED 2
#define RED_LED TB3CCR1
#define GREEN_LED TB3CCR2
#define BLUE_LED TB3CCR3
```

## Prototypes and Main Function:
This section consisted of the prototypes used along with the main function of the code.
```c
#include <msp430.h>

// State starting variable
char LEDstate = RED_TO_GREEN;

void LEDSetup();
void TimerSetup();

void main()
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    LEDSetup();                         // Initialize LED pins
    TimerSetup();                       // Initialize timer

    PM5CTL0 &= ~LOCKLPM5;               // Unlock LPM5

    __bis_SR_register(LPM0_bits | GIE); // Enter LPM0 with interrupts enabled
}
```

## Initialization:
This section is where the prototypes used were intialized. P6.0-P6.2 were intialized as outputs in order to drive RGB LED. Configured PWM Timer to switch color every 1s.
```c
// Configure LED pins and interrupts
void LEDSetup(){
    P6DIR |= BIT0;              // Set output for RGB LED
    P6OUT &= ~BIT0;             // Initialize pin to power-off state
    P6SEL0 |= BIT0;             // Select PWM
    P6SEL1 &= ~BIT0;            // Clear bit
    P6IE |= BIT0;               // Enable interrupt

    P6DIR |= BIT1;              // Set output for RGB LED
    P6OUT &= ~BIT1;             // Initialize pin to power-off state
    P6SEL0 |= BIT1;             // Select PWM
    P6SEL1 &= ~BIT1;            // Clear bit
    P6IE |= BIT1;               // Enable interrupt

    P6DIR |= BIT2;              // Set output for RGB LED
    P6OUT &= ~BIT2;             // Initialize pin to power-off state
    P6SEL0 |= BIT2;             // Select PWM
    P6SEL1 &= ~BIT2;            // Clear bit
    P6IE |= BIT2;               // Enable interrupt
}

// Configure timer for PWM and interrupts
void TimerSetup(){
    TB3CCR0 = 1000 - 1;                         // Set PWM period
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR;    // Configure timer: SMCLK, up mode, clear TBR

    TB3CCTL1 = OUTMOD_7;                        // (RED) Set output mode for CCR1-reset/set
    TB3CCTL2 = OUTMOD_7;                        // (GREEN) Set output mode for CCR2-reset/set
    TB3CCTL3 = OUTMOD_7;                        // (BLUE) Set output mode for CCR3-reset/set

    RED_LED = 1000 - 1;                         // Set initial duty cycle for RGB LED
    // Set the initial duty cycle values for the green and blue LEDs to 0%
    GREEN_LED = 0;                              // Green LED off
    BLUE_LED = 0;                               // Blue LED off

    TB1CCR0 = 1;                                // Set initial period for TB1
    TB1CTL = TBSSEL_1 | MC_2 | ID_3 | TBCLR | TBIE; // Configure timer: ACLK, continuous mode, divider /8, clear TBR, enable interrupt
    TB1CCTL0 |= CCIE;                           // Enable TB1 CCR0 overflow interrupt
}
```

## Timer Interrupts:
This section is to control the different color states moving from red, to green, to blue, and the colors in between.
```c
// Timer B1 ISR for handling LED transitions
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    switch(LEDstate){
    case RED_TO_GREEN:
        RED_LED--;                   // Decrease RED to 0%
        GREEN_LED++;                 // Increase GREEN to 50%
        if (RED_LED == 0)
            LEDstate = GREEN_TO_BLUE;
        break;
    case GREEN_TO_BLUE:
        GREEN_LED--;                 // Decrease GREEN to 0%
        BLUE_LED++;                  // Increase BLUE to 50%
        if (GREEN_LED == 0)
            LEDstate = BLUE_TO_RED;
        break;
    case BLUE_TO_RED:
        BLUE_LED--;                   // Decrease BLUE to 0%
        RED_LED++;                    // Increase RED to 50%
        if (BLUE_LED == 0)
            LEDstate = RED_TO_GREEN;
        break;
    }

    // Check if the timer value TB3R is greater than or equal to 60000
    if(TB3R >= 60000)
        TB3R = 1;                     // If so, reset TB3R to 1 to avoid overflow (max value is 65535)

    // Increment TB1CCR0 by 20 to adjust the timer compare value
    // This determines the speed of the LED color fade transition
    TB1CCR0 += 20;
}
```
