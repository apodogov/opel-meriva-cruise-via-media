// ATmega88:
// System Clock: 8 MHz. Target: 25 ms (40 Hz)
// 88 us - /64 /64 /49 /0 = 25.088 ms (39.86 Hz)
// Arduino:
// System Clock: 16 MHz. Target: 25 ms (40 Hz)
// 40 us - /2 /1024 /195 /0 = 24.96 ms (40.064 Hz)

#include "Timer.h"

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef DEBUG
#define TimerDivider	195
#else
#define TimerDivider	49
#endif

void Timer_Init()
{
	TCCR2A = (1 << WGM21); // CTC, TOP = OCR2A
	OCR2A = TimerDivider -1; // Timer Divider
	TIMSK2 = (1 << OCIE2A); // Compare match Interrupt
#ifdef DEBUG
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); // Enable Timer with Prescaler /1024
#else
	TCCR2B = (1 << CS22); // Enable Timer with Prescaler /64
#endif
}

ISR(TIMER2_COMPA_vect)
{
	OnTimer();
}