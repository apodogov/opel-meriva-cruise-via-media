#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "Buttons.h"

void Configurator_Enter();

void Configurator_Tick();

uint8_t Configurator_GetVolumeTogglePeriod();

bool HandleTurnButtonPress(TurnButton button);

bool HandleTurnButtonRelease();

bool HandleMediaButtonPress(MediaButton button);

bool HandleMediaButtonRelease();