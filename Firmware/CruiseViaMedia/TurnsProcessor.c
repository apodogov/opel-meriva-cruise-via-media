#include "TurnsProcessor.h"

#include <stdio.h>
#include <stdbool.h>

#include "Log.h"

#include "Buttons.h"
#include "Input.h"
#include "Output.h"

#include "Configurator.h"
#include "Settings.h"

bool isInButtonActive = false;
uint8_t remainDuration = 0;

void TurnsProcessor_Tick()
{
	if (remainDuration > 0)
	{
		if (--remainDuration == 0)
		{
			LogLine("Turn: Delay passed");
			if (!isInButtonActive)
			{
				ReleaseTurnButton();
			}
		}
	}
}

void OnTurnButtonPressed(TurnButton button)
{
	if (HandleTurnButtonPress(button)) return;
	
	// If user quickly changed direction of turn lights - cancel current turn, show new turn (must do this), but cancel minimum duration of the second turn for case if used tried to cancel first turn.
	// If user pressed the same turn second time during minimum period, just cancel this minimum period and let user control duration.
	remainDuration = (remainDuration != 0) ? 0 : GetMinTurnDuration();
	LogText("Turn: Delay set: ");
	LogIntLn(remainDuration);

	isInButtonActive = true;

	PressTurnButton(button);
}

void OnTurnButtonReleased()
{
	if (HandleTurnButtonRelease()) return;
	
	isInButtonActive = false;
	if (remainDuration == 0)
	{
		ReleaseTurnButton();
	}
	else
	{
		LogText("Turn: Delay remains: ");
		LogIntLn(remainDuration);
	}
}