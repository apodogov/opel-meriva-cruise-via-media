#include "MediaProcessor.h"

#include <stdio.h>
#include <stdbool.h>

#include "Buttons.h"
#include "Input.h"
#include "Output.h"

#include "Configurator.h"
#include "Settings.h"

MediaButton pressedInButton = MediaButtonNone;

uint8_t ticksToToggle = 0;
bool isActivePeriod = false;

bool enterConfig = false;

void MediaProcessor_Tick()
{
	if (ticksToToggle != 0)
	{
		if (--ticksToToggle == 0)
		{
			ticksToToggle = Configurator_GetVolumeTogglePeriod();
			isActivePeriod = !isActivePeriod;
			
			// Now only media buttons support toggling. To support others one more condition is required.
			if (isActivePeriod) PressMediaButton(pressedInButton);
			else ReleaseMediaButton();
		}
	}
}

void StartToggling()
{
	ticksToToggle = Configurator_GetVolumeTogglePeriod();
	isActivePeriod = true;
}

void StopToggling()
{
	ticksToToggle = 0;
}

// To avoid confusion, do not press simultaneously Media and Cruise buttons.
// This is possible during potentially supported two-buttons combinations when the first button presses
// one-side button (e.g. Cruise) while the second button presses other-side button (e.g. Media).
void PressOnlyMediaButton(MediaButton button)
{
	PressMediaButton(button);
	ReleaseCruiseButton();
}

void PressOnlyCruiseButton(CruiseButton button)
{
	PressCruiseButton(button);
	ReleaseMediaButton();
}

void OnMediaButtonPressed(MediaButton button)
{
	StopToggling();
	if (HandleMediaButtonPress(button)) return;
	
	if (pressedInButton != MediaButtonNone)
	{
		// Second button was pressed before previous one was released.
		if (pressedInButton == button)
		{
			// This means that the second button was released. Do not need to press the first button again or leave the second button pressed.
			ReleaseMediaButton();
			ReleaseCruiseButton();
		}

		if (pressedInButton == MediaButtonSource)
		{
			if (button == MediaButtonVolumeDown) PressOnlyMediaButton(MediaButtonNext);
			else if (button == MediaButtonVolumeUp) PressOnlyMediaButton(MediaButtonPrevious);
		}
		else if (pressedInButton == MediaButtonVolumeUp)
		{
			if (button == MediaButtonVolumeDown) PressOnlyMediaButton(MediaButtonSource);
		}
		else if (pressedInButton == MediaButtonArrowUp)
		{
			if (button == MediaButtonVolumeUp)
			{
				enterConfig = true;
			}
		}
		
		// Just ignore any other double button combinations until all buttons are released.
		return;
	}
	
	// Single (or first) button pressed. Map it to other button if required.
	if (button == MediaButtonSource)
	{
		PressOnlyMediaButton(MediaButtonArrowUp);
	}
	else if (button == MediaButtonNext)
	{
		PressOnlyCruiseButton(CruiseButtonSet);
	}
	else if (button == MediaButtonPrevious)
	{
		PressOnlyCruiseButton(CruiseButtonResume);
	}
	else if (button == MediaButtonArrowUp)
	{
		PressOnlyCruiseButton(CruiseButtonOff);
	}
	else
	{
		// Other buttons (Volume Up/Down) are not re-mapped and require quick toggling.
		PressOnlyMediaButton(button);
		StartToggling();
	}
	
	pressedInButton = button;
}

void OnMediaButtonReleased()
{
	StopToggling();
	if (HandleMediaButtonRelease()) return;
	
	ReleaseMediaButton();
	ReleaseCruiseButton();
	pressedInButton = MediaButtonNone;
	
	if (enterConfig)
	{
		enterConfig = false;
		Configurator_Enter();
	}
}