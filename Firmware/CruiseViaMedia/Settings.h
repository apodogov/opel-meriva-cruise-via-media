#pragma once

#include <stdio.h>
#include <stdbool.h>

#define SettingsMinTurnDurationDisabled			0
#define SettingsMinTurnDurationMaxValue			254

#define SettingsVolumeTogglePeriodDisabled		0
#define SettingsVolumeTogglePeriodMinValue		1
#define SettingsVolumeTogglePeriodMaxValue		10

typedef struct Settings {
	uint8_t minTurnDuration;
	uint8_t volumeTogglePeriod;
} Settings;

void Settings_Load();

void Settings_Save(Settings newSettings);

uint8_t GetMinTurnDuration();

uint8_t GetVolumeTogglePeriod();