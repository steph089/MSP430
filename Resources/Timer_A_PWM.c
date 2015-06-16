//************************************************************************
// MSP430 Demo - Timer_A, PWM TA1, Up Mode, SMCLK 
//
// Description: This program generates one PWM output on P1.2 using
// Timer_A configured for up mode. The value in CCR0, 1000-1, defines the
// PWM period and the value in CCR1 the PWM duty cycles. Using SMCLK,
// the timer frequenciy is about 1.1kHz with a 25% duty cycle on P1.2.
// Normal operating mode is LPM0.
// MCLK = SMCLK = default DCO (about 1.1MHz).
//
//        MSP430G2231 
//        ----------------- 
//   /|\ |                 | 
//    |  |                 | 
//     --|RST      P1.2/TA1|--> CCR1 - 75% PWM
//       |                 |
//       |                 |
//
// M.Buccini / L. Westlund
// Texas Instruments, Inc
// October 2005
// Built with CCE Version: 3.2.0 and IAR Embedded Workbench Version: 3.40A
//
// Modified by NJC for MSP430LaunchPad.com - July 2010
//retrieved from
//http://www.msp430launchpad.com/2010/07/timers-and-clocks-and-pwm-oh-my.html
//************************************************************************
 
 
#include "msp430.h"
 
void main(void)
{
 WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
 
 P1DIR |= BIT2;             // P1.2 to output
 P1SEL |= BIT2;             // P1.2 to TA0.1
 
 CCR0 = 1000-1;             // PWM Period
 CCTL1 = OUTMOD_7;          // CCR1 reset/set
 CCR1 = 250;                // CCR1 PWM duty cycle
 TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode
 
 _BIS_SR(LPM0_bits);        // Enter LPM0
 }
 
 