/*
 * HardwarePWMExample.c
 *
 *  Created on: Feb 18, 2023
 *      Author: Russell Trafford
 *
 *      This example configures P6.0 as a PWM output as controlled by TimerB3.
 *
 *      You will need to connect an external LED or use a scope to see the effect of the Duty Cycle.
 *
 *      You can control the DutyCycle by changing the values in the TB3CCR1 register
 */


#include <msp430.h>


void LEDSetup();
void TimerSetup();


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

    LEDSetup();
    TimerSetup();

    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(LPM0_bits);             // Enter LPM0
    __no_operation();                         // For debugger
}


// Initialization
void LEDSetup() {
    P6DIR |= BIT0;                            // P6.0 output
    P6OUT &= ~BIT0;
    P6SEL0 |= BIT0;
    P6SEL1 &= ~BIT0;                          // P6.0 options select
    P6IE |= BIT0;                             // P6.0 interrupt enabled


    P6DIR |= BIT1;                            // P6.1 output
    P6OUT &= ~BIT1;
    P6SEL0 |= BIT1;
    P6SEL1 &= ~BIT1;                          // P6.1 options select
    P6IE |= BIT1;                             // P6.1 interrupt enabled


    P6DIR |= BIT2;                            // P6.2 output
    P6OUT &= ~BIT2;
    P6SEL0 |= BIT2;
    P6SEL1 &= ~BIT2;                          // P6.2 options select
    P6IE |= BIT2;                             // P6.2 interrupt enabled
}


void TimerSetup() {
    // Initialize
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR;  // SMCLK, up mode, clear TBR
    TB3CCR0 = 1000-1;                         // PWM Period
    TB3CCTL1 = OUTMOD_7;                      // CCR1 reset/set
    TB3CCTL2 = OUTMOD_7;                      // CCR2 reset/set
    TB3CCTL3 = OUTMOD_7;                      // CCR3 reset/set

    // Setup TimerB3 on Startup
    TB3CCR1 = 1000-1;                         // PWM Period
    TB3CCR2 = 0;
    TB3CCR3 = 0;

    // Setup TimerB1 Compare IRQ
    TB1CTL = TBSSEL__ACLK | MC__UP | ID__3 | TBCLR | TBIE;
    TB1CCTL0 |= CCIE;
    TB1CCR0 = 1;

}

// Interrupts


#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{

}



