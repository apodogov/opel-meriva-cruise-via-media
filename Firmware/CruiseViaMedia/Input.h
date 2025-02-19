#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "Buttons.h"

void Input_Init();

void Input_Tick();

bool IsInputReadingInProgress();

extern void OnMediaButtonPressed(MediaButton button);

extern void OnMediaButtonReleased();

extern void OnTurnButtonPressed(TurnButton button);

extern void OnTurnButtonReleased();