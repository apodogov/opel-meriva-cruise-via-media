#include "Input.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Log.h"

#define MediaInMux			5
#define TurnInMux			4

#define MinAdcValue					0
#define MaxAdcValue					1023

#define MediaVolumeDownAdcValue		25
#define MediaVolumeUpAdcValue		48
#define MediaSourceAdcValue			81
#define MediaNextAdcValue			126
#define MediaPreviousAdcValue		198
#define MediaArrowUpAdcValue		316
#define MediaNothingAdcValue		539

#define TurnRightAdcValue			44
#define TurnLeftAdcValue			130
#define TurnNothingAdcValue			317

#define StabilizationDelay			1

typedef struct AdcRange {
	uint16_t to;
	Button button;
} AdcRange;

AdcRange MediaAdcRanges[] = {
	{ .to = ((MinAdcValue + MediaVolumeDownAdcValue) / 2), .button = ButtonError },
	{ .to = ((MediaVolumeDownAdcValue + MediaVolumeUpAdcValue) / 2), .button = MediaButtonVolumeDown},
	{ .to = ((MediaVolumeUpAdcValue + MediaSourceAdcValue) / 2), .button = MediaButtonVolumeUp},
	{ .to = ((MediaSourceAdcValue + MediaNextAdcValue) / 2), .button = MediaButtonSource},
	{ .to = ((MediaNextAdcValue + MediaPreviousAdcValue) / 2), .button = MediaButtonNext},
	{ .to = ((MediaPreviousAdcValue + MediaArrowUpAdcValue) / 2), .button = MediaButtonPrevious},
	{ .to = ((MediaArrowUpAdcValue + MediaNothingAdcValue) / 2), .button = MediaButtonArrowUp},
	{ .to = ((MediaNothingAdcValue + MaxAdcValue) / 2), .button = MediaButtonNone},
};

AdcRange TurnAdcRanges[] = {
	{ .to = ((MinAdcValue + TurnRightAdcValue) / 2), .button = ButtonError },
	{ .to = ((TurnRightAdcValue + TurnLeftAdcValue) / 2), .button = TurnButtonRight},
	{ .to = ((TurnLeftAdcValue + TurnNothingAdcValue) / 2), .button = TurnButtonLeft},
	{ .to = ((TurnNothingAdcValue + MaxAdcValue) / 2), .button = TurnButtonNone},
};

bool isReadingInProgress = false;
bool isReadingMedia = false;

MediaButton mediaButtonState = MediaButtonNone;
TurnButton turnButtonState = TurnButtonNone;

uint16_t lastMediaValue = 0;
uint16_t lastTurnValue = 0;

uint8_t mediaDelay = 0;
uint8_t turnDelay = 0;

void SwitchAdcInput()
{
	isReadingMedia = !isReadingMedia;
	if (isReadingMedia) ADMUX = MediaInMux;
	else ADMUX = TurnInMux;

#ifdef DEBUG
	// Arduino board doesn't have AREF pin connected, so use AVCC as reference voltage.
	ADMUX |= (1 << REFS0);
#endif
}

void Input_Init()
{
	// If some pins are unused, it is recommended to ensure that these pins have a defined level. At least by internal pull-ups.
	PORTC = 0b11001111;
	DDRC = 0x00;

	ADCSRB = 0x00;
}

void Input_Tick()
{
	isReadingInProgress = true;
	SwitchAdcInput();

	// Recommended frequency is 50 kHz - 200 kHz. System Clock is 8 MHz /64 = 125 kHz. So, prescaler /2 gives 62.5 kHz.
#ifdef DEBUG
	// For Arduino board: System Clock is 16 MHz /2 =  8 MHz. So, prescaler /128 gives 62.5 kHz.
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
#else
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADIE) | (1 << ADPS0);
#endif
}

bool IsInputReadingInProgress()
{
	return isReadingInProgress;
}

Button FindButtonMatch(uint16_t value, AdcRange* candidates, uint8_t count)
{
	for (uint8_t i = 0; i < count; i++)
	{
		AdcRange range = candidates[i];
		if (value <= range.to) return range.button;
	}
	
	return ButtonError;
}

void ProcessMediaAdcValue(uint16_t value)
{
	MediaButton btn = FindButtonMatch(value, (AdcRange*)&MediaAdcRanges, sizeof(MediaAdcRanges) / sizeof(AdcRange));
	
	if (btn == MediaButtonError)
	{
		// TODO: add statistics
		if (value != lastMediaValue)
		{
			LogText("In: Media: Unknown ADC: ");
			LogIntLn(value);
		}
		
		btn = MediaButtonNone;
	}
	
	lastMediaValue = value;
	
	if (btn != mediaButtonState)
	{
		LogText("In: Media: ");
		LogInt(btn);
		LogText(" ADC: ");
		LogIntLn(value);
		
		if (mediaDelay == 0)
		{
			mediaDelay = StabilizationDelay;
		}
		else
		{
			if (--mediaDelay == 0)
			{
				mediaButtonState = btn;
				if (btn != MediaButtonNone) OnMediaButtonPressed(btn);
				else OnMediaButtonReleased();
			}
		}
	}
	else
	{
		mediaDelay = 0;
	}
}

void ProcessTurnAdcValue(uint16_t value)
{
	TurnButton btn = FindButtonMatch(value, (AdcRange*)&TurnAdcRanges, sizeof(TurnAdcRanges) / sizeof(AdcRange));

	if (btn == TurnButtonError)
	{
		// TODO: add statistics
		if (value != lastTurnValue)
		{
			LogText("In: Turn: Unknown ADC: ");
			LogIntLn(value);
		}
		
		btn = TurnButtonNone;
	}
	
	lastTurnValue = value;
	
	if (btn != turnButtonState)
	{
		LogText("In: Turn: ");
		LogInt(btn);
		LogText(" ADC: ");
		LogIntLn(value);
		
		if (turnDelay == 0)
		{
			turnDelay = StabilizationDelay;
		}
		else
		{
			if (--turnDelay == 0)
			{
				turnButtonState = btn;
				if (btn != TurnButtonNone) OnTurnButtonPressed(btn);
				else OnTurnButtonReleased();
			}
		}
	}
	else
	{
		turnDelay = 0;
	}
}

ISR(ADC_vect)
{
	uint16_t value = ADC;
	if (isReadingMedia) ProcessMediaAdcValue(value);
	else ProcessTurnAdcValue(value);
	isReadingInProgress = false;
}