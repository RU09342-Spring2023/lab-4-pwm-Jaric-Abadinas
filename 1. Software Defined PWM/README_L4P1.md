# README file:
.c file: Part1.c

## Task:
Generate a 1 kHz PWM signal with a duty cyle between 0% and 100%. When processor starts up, both red and green LEDs should be at 50% duty cycle. Upon pressing a button, the duty cycle of that respective LED should increase by 10%. Once you have reached 100%, duty cycle should go back down to 0%.

## Prototypes and Main Function:
This section consisted of the prototypes used along with the main function of the code.
```c
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
```

## Initialization:
This section is where the prototypes used were intialized. LED setups for P1.0 Red LED and P6.6 Green LED were set as outputs. Button setups for P2.3 and P4.3 were set to inputs. Two timers were created for their respective LEDs where SMCLK was used for a frequency of 1 Mhz for easy conversions. Both LEDs are to be set at 50% at startup.
```c
void LEDSetup() {
    // Configure Red LED on P1.0 Output
    P1DIR |= BIT0;                          // Set P1.0 to output direction
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state

    // Configure Green LED on P6.6 as Output
    P6DIR |= BIT6;                          // Set P6.6 to output direction
    P6OUT &= ~BIT6;                         // Clear P6.6 output latch for a defined power-on state
}

void ButtonSetup() {
    // Configure button P2.3
    P2DIR &= ~BIT3;                         // Set P2.3 to input direction
    P2REN |= BIT3;                          // Enable the pull up/down resistor for Pin 2.3
    P2OUT |= BIT3;                          // Pull-up resistor
    P2IES &= ~BIT3;                         // P2.3 L --> H edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled

    // Configure button P4.1
    P4DIR &= ~BIT1;                         // Set P4.1 to input direction
    P4REN |= BIT1;                          // Enable the pull up/down resistor for Pin 4.1
    P4OUT |= BIT1;                          // Pull-up resistor
    P4IES &= ~BIT1;                         // P4.1 L --> H edge
    P4IE |= BIT1;                           // P4.1 interrupt enabled
}

void TimerB0Setup() {
    //PWM Generator
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBIE;      // SMCLK, continuous mode, clear TBR, enable interrupt
    TB0CCTL1 |= CCIE;
    TB0CCR1 = 500;
    TB0CCR0 = 1000;                              // PWM Time Period/ frequency (1 kHz)
}

void TimerB1Setup() {
    //PWM Generator
    TB1CTL = TBSSEL__SMCLK | MC__UP | TBIE;      // SMCLK, continuous mode, clear TBR, enable interrupt
    TB1CCTL1 |= CCIE;
    TB1CCR1 = 500;
    TB1CCR0 = 1000;                              // PWM Time Period/ frequency (1 kHz)
}
```

## Button Interrupts:
This section is to show which button controlled which LED and once button pressed, timer interrupted to do the intended task. Button P2.3 controls red LED and button P4.1 controls the green LED. If a button pressed and if the duty cycle is 100%, set the brightness down to 0%; else add 10% to the respective LED.
```c
// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG
    if  (TB0CCR1 >= 1000)                   // If Duty Cycle 100%
        TB0CCR1 = 0;                        // Set brightness almost 0%
    else
        TB0CCR1 += 100;                     // else, add 10%
}

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG
    if  (TB1CCR1 >= 1000)                   // If Duty Cycle 100%
        TB1CCR1 = 0;                        // Set brightness almost 0%
    else
        TB1CCR1 += 100;                     // else, add 10%
}
```

## Timer Interrupts:
This section is where the blinking happens to control the brightness or the PWM section. Based on the timer of CCR1, the LED will be turned off for that certain time and then turned back on. This repeated action of blinking an LED is what causes the brightness.
```c
// Timer0_B1 Interrupt Vector (TBIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TB0IV,TB0IV_TBIFG))
    {
        case TB0IV_NONE:
            break;                               // No interrupt
        case TB0IV_TBCCR1:
            P1OUT &= ~BIT0;
            break;                               // CCR1 Set the pin to a 0
        case TB0IV_TBCCR2:
            break;                               // CCR2 not used
        case TB0IV_TBIFG:
            P1OUT |= BIT0;                       // overflow Set the pin to a 1
            break;
        default:
            break;
    }
}

// Timer1_B1 Interrupt Vector (TBIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_B1_VECTOR
__interrupt void TIMER1_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_B1_VECTOR))) TIMER1_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TB1IV,TB1IV_TBIFG))
    {
        case TB1IV_NONE:
            break;                               // No interrupt
        case TB1IV_TBCCR1:
            P6OUT &= ~BIT6;
            break;                               // CCR1 Set the pin to a 0
        case TB1IV_TBCCR2:
            break;                               // CCR2 not used
        case TB1IV_TBIFG:
            P6OUT |= BIT6;                       // overflow Set the pin to a 1
            break;
        default:
            break;
    }
}
```
