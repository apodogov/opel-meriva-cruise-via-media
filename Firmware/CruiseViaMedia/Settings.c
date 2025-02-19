#include "Settings.h"

#include <avr/eeprom.h>

#define SettingsAddress					0

// Duration is measured in interrupts number. Interval between interrupts is 25 ms. So value 80 results 2000 ms.
// There is some inaccuracy because a) ADC is measured not exactly at the interrupt time; b) different inputs are measured at different intrrupts.
#define DefaultMinimumTurnDuration		(2000 / 25)

// Duration is measured in interrupts number. Interval between interrupts is 25 ms. So value 6 results 150 ms (6.7 toggles, 3.3 clicks per second).
#define DefaultTogglePeriod				6

Settings settings;

void Settings_Load()
{
	eeprom_read_block(&settings, SettingsAddress, sizeof(Settings));
	
	// If it is the first start, use default settings.
	if (settings.minTurnDuration > SettingsMinTurnDurationMaxValue) settings.minTurnDuration = DefaultMinimumTurnDuration;
	if (settings.volumeTogglePeriod > SettingsVolumeTogglePeriodMaxValue) settings.volumeTogglePeriod = DefaultTogglePeriod;
}

void Settings_Save(Settings newSettings)
{
	settings.minTurnDuration = newSettings.minTurnDuration;
	settings.volumeTogglePeriod = newSettings.volumeTogglePeriod;

	eeprom_write_block(&settings, SettingsAddress, sizeof(Settings));
}

uint8_t GetMinTurnDuration()
{
	return settings.minTurnDuration;
}

uint8_t GetVolumeTogglePeriod()
{
	return settings.volumeTogglePeriod;
}