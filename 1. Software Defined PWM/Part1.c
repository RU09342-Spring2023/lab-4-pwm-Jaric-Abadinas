/*
 * Part1.c
 *
 *  Created on: Feb 23, 2023
 *      Author: Jaric Abadinas
 *
 *  This code generates a 1kHz PWM signal with a duty cycle between 0% and 100%.
 *  Upon the processor starting up, PWM both of the on-board LEDs at a 50% duty cycle.
 *  Upon pressing the on-board buttons, the duty cycle of the LEDs should increase by 10%,
 *  based on which button pressed. Once you have reached 100%,
 *  duty cycle should go back to 0% on the next button press.
 *
 *  Button 2.3 controls LED P1.0
 *  Button 4.1 controls LED P6.6
 */


#include <msp430.h>


void LEDSetup();
void ButtonSetup();
void TimerB0Setup();
void TimerB1Setup();


void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // turn on GPIO

    LEDSetup(); // Initialize our LEDS
    ButtonSetup();  // Initialize our button
    TimerB0Setup(); // Initialize Timer0
    TimerB1Setup(); // Initialize Timer1

    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM0 w/ interrupt
}

/*
 * Initialization
 */

void LEDSetup() {
    // Configure Red LED on P1.0 Output
    P1OUT |= BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

    // Configure Green LED on P6.6 as Output
    P6OUT |= BIT6;                         // Clear P6.6 output latch for a defined power-on state
    P6DIR |= BIT6;                          // Set P6.6 to output direction
}

void ButtonSetup() {
    // Configure button P2.3
    P2REN |= BIT3;                          // Enable the pull up/down resistor for Pin 2.3
    P2OUT |= BIT3;                          // Pull-up resistor
    P2IES |= BIT3;                          // P2.3 High --> Low edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled

    // Configure button P4.1
    P4REN |= BIT1;                          // Enable the pull up/down resistor for Pin 4.1
    P4OUT |= BIT1;                          // Pull-up resistor
    P4IES |= BIT1;                          // P4.1 High --> Low edge
    P4IE |= BIT1;                           // P4.1 interrupt enabled
}

void TimerB0Setup() {
    //PWM Generator
    TB0CTL = TBSSEL_2 | MC_2 | TBCLR | TBIE;      // SMCLK, continuous mode, clear TBR, enable interrupt
    TB0CCTL1 |= CCIE;
    TB0CCR1 = 12000;             // PWM Time Period/ frequency (1 kHz)
    // TB0CCR1 = 500;                // PWM Duty cycle is 50%
    // Set up Timer Compare IRQ
    //TB0CCTL0 &= ~CCIFG;
}

void TimerB1Setup() {
    //PWM Generator
    TB1CTL = TBSSEL_2 | MC_2 | TBCLR | TBIE;      // SMCLK, continuous mode, clear TBR, enable interrupt
    TB1CCTL1 |= CCIE;
    TB1CCR1 = 12000;             // PWM Time Period/ frequency (1 kHz)
    // TB1CCR1 = 500;                // PWM Duty cycle is 50%
    // Set up Timer Compare IRQ
    //TB1CCTL0 &= ~CCIFG;
}


/*
 * Interrupts
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG
/*
    if (P2IES & BIT3) {
        if (TB0CCR1 == 500)
           TB0CCR1 = 600;
        else if (TB0CCR1 == 600)
            TB0CCR1 = 700;
        else {
            TB0CCR1 = 700;
        }
        P2IES &= ~BIT3;
    }

    else if (!(P2IES & BIT3)) {// @TODO Fill in this argument within the If statement to check if the interrupt was triggered off a falling edge.
        TB1CCR0 = TB1CCR0;
        P2IES |= BIT3;
    }
*/
}

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG
}

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
    switch(__even_in_range(TB0IV,TB0IV_TBIFG))
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
