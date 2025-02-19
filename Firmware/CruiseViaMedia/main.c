// Fuses:
// Extended: F9 (default)
// High: DF -> D7
//     Preserve EEPROM
// Low: 62

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "Log.h"
#include "Timer.h"
#include "Input.h"
#include "Output.h"

#include "MediaProcessor.h"
#include "TurnsProcessor.h"
#include "Settings.h"
#include "Configurator.h"

int main(void)
{
	// Turn off all periphery to reduce power consumption.

	// Analog to Digital Converter is used.

	// Analog Comparator.
	ACSR |= (1 << ACD);

	// Brown-out Detector is disabled by fuses.
	// Internal Voltage Reference is disabled when ADC, AC and BOD are disabled.

	// Watchdog Timer.
	wdt_disable();

	// Digital input buffers.
	DIDR1 = (1 << AIN1D) | (1 << AIN0D);
	DIDR0 = (1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D);

	// On-chip Debug System is disabled by fuses.

	// Power Reduction Register.
#ifdef LOG
	// USART is used for logging.
	PRR = (1 << PRTWI) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRSPI);
#else
	PRR = (1 << PRTWI) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0);
#endif
	
	// Delay slowing down CPU clock, so ISP can program chip when it runs on the fast enough frequency.
	// Startup frequency is 1 MHz.
	_delay_ms(1000);

#ifdef DEBUG
	clock_prescale_set(clock_div_2); // For Arduino prototype board (with logging enabled).
#else
	clock_prescale_set(clock_div_64); // For internal RC oscillator.
#endif
	
	Log_Init();
	LogLine("Start");

	Settings_Load();
	Timer_Init();
	Input_Init();
	Output_Init();
	
	sei();
	
    while (true)
    {
#ifndef DEBUG
		if (IsInputReadingInProgress())
		{
			set_sleep_mode(SLEEP_MODE_ADC);
			sleep_mode();
		}
		else
		{
			set_sleep_mode(SLEEP_MODE_PWR_SAVE);
			sleep_mode();
		}
#endif
    }
}

void OnTimer()
{
	MediaProcessor_Tick();
	TurnsProcessor_Tick();
	Configurator_Tick();
	Input_Tick();
}