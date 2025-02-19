#include "Configurator.h"

#include "Log.h"

#include "Settings.h"

#include "Buttons.h"
#include "Output.h"

#define EnterBlinkDuration			(2000 / 25)
#define ExitSaveBlinkDuration		(1200 / 25)
#define ExitNoSaveBlinkDuration		(500 / 25)

#define ShortBlinkDuration			(100 / 25)
#define TwoBlinksDuration			(1200 / 25)

typedef enum ConfigItem {
	ConfigItemNone = 0,
	ConfigItemMenu,
	ConfigItemErrors,
	ConfigItemMinTurnDuration,
	ConfigItemVolumeToggleSpeed,
} ConfigItem;

ConfigItem currentConfigItem = ConfigItemNone;

bool isDirty = false;
Settings dirtySettings;

bool propagateVolume = false;

uint8_t blinkPeriod = 0;
uint8_t blinkExitPeriod = 0;

void Blink(uint8_t ticks)
{
	blinkPeriod = ticks;
	PressTurnButton(TurnButtonRight);
}

void BlinkAndExit(uint8_t ticks)
{
	blinkExitPeriod = ticks;
	PressTurnButton(TurnButtonRight);
}

void Configurator_Enter()
{
	LogLine("Config: Enter");
	
	currentConfigItem = ConfigItemMenu;
	
	dirtySettings.minTurnDuration = GetMinTurnDuration();
	dirtySettings.volumeTogglePeriod = GetVolumeTogglePeriod();
	isDirty = false;
	
	// To indicate that configuration entered, blink few times.
	Blink(EnterBlinkDuration);
}

void Configurator_Tick()
{
	if (blinkPeriod)
	{
		if (--blinkPeriod == 0) ReleaseTurnButton();
	}
	
	if (blinkExitPeriod)
	{
		if (--blinkExitPeriod == 0)
		{
			LogLine("Config: Exit");
	
			ReleaseTurnButton();
			currentConfigItem = ConfigItemNone;
		}
	}
	
	if (currentConfigItem == ConfigItemMinTurnDuration)
	{
		if (dirtySettings.minTurnDuration < SettingsMinTurnDurationMaxValue) dirtySettings.minTurnDuration++;
		if (dirtySettings.minTurnDuration == SettingsMinTurnDurationMaxValue) ReleaseTurnButton();
	}
}

uint8_t Configurator_GetVolumeTogglePeriod()
{
	if (propagateVolume) return dirtySettings.volumeTogglePeriod;
	
	return GetVolumeTogglePeriod();
}

bool HandleTurnButtonPress(TurnButton button)
{
	if (currentConfigItem == ConfigItemNone) return false;
	
	if (currentConfigItem == ConfigItemMenu)
	{
		currentConfigItem = ConfigItemMinTurnDuration;

		dirtySettings.minTurnDuration = 0;
		isDirty = true;

		// Indicate measurement process by left turn.
		PressTurnButton(TurnButtonLeft);
	}
	
	return true;
}

bool HandleTurnButtonRelease()
{
	if (currentConfigItem == ConfigItemNone) return false;
	
	if (currentConfigItem == ConfigItemMinTurnDuration)
	{
		ReleaseTurnButton();
		currentConfigItem = ConfigItemMenu;

		// Indicate that new value was measured - repeat it.
		Blink(dirtySettings.minTurnDuration);
	}
	
	return true;
}

bool HandleMediaButtonPress(MediaButton button)
{
	if (currentConfigItem == ConfigItemNone) return false;
	
	if (currentConfigItem == ConfigItemMenu)
	{
		if (button == MediaButtonArrowUp)
		{
			// Cancel settings change.
			BlinkAndExit(ExitNoSaveBlinkDuration);
		}
		
		if (button == MediaButtonSource)
		{
			if (isDirty)
			{
				Settings_Save(dirtySettings);
				
				BlinkAndExit(ExitSaveBlinkDuration);
			}
			else
			{
				BlinkAndExit(ExitNoSaveBlinkDuration);
			}
		}

		if (button == MediaButtonNext)
		{
			if (dirtySettings.volumeTogglePeriod > SettingsVolumeTogglePeriodMinValue)
			{
				dirtySettings.volumeTogglePeriod--;
				isDirty = true;
				Blink(ShortBlinkDuration);
			}
			else
			{
				Blink(TwoBlinksDuration);
			}
		}

		if (button == MediaButtonPrevious)
		{
			if (dirtySettings.volumeTogglePeriod < SettingsVolumeTogglePeriodMaxValue)
			{
				dirtySettings.volumeTogglePeriod++;
				isDirty = true;
				Blink(ShortBlinkDuration);
			}
			else
			{
				Blink(TwoBlinksDuration);
			}
		}
		
		if ((button == MediaButtonVolumeUp) || (button == MediaButtonVolumeDown))
		{
			// Allow regular media processor to control volume buttons, so user can adjust volume speed and immediately test it.
			propagateVolume = true;
			return false;
		}
	}
	
	return true;
}

bool HandleMediaButtonRelease()
{
	if (currentConfigItem == ConfigItemNone) return false;
	
	if (propagateVolume)
	{
		// Allow regular media processor to see that volume buttons were released.
		propagateVolume = false;
		return false;
	}
	
	return true;
}