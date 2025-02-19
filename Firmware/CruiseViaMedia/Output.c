#include "Output.h"

#include <stdio.h>
#include <avr/io.h>

#include "Log.h"

#define MediaPortDDR		DDRD
#define MediaPort			PORTD
#ifdef LOG
#define MediaPortMask	0b00111100
#else
#define MediaPortMask	0b00111111
#endif

#define TurnPortDDR			DDRD
#define TurnPort			PORTD
#define TurnPortMask		0b11000000

#define CruisePortDDR		DDRB
#define CruisePort			PORTB
#define CruisePortMask		0b00000111

void Output_Init()
{
	// If some pins are unused, it is recommended to ensure that these pins have a defined level. At least by internal pull-ups.
	PORTB = ~CruisePortMask;
	DDRB = CruisePortMask;

	PORTD = (uint8_t)(~(MediaPortMask | TurnPortMask));
	DDRD = (MediaPortMask | TurnPortMask);
	
#ifdef LOG
	PORTC &= ~(0b11);
	DDRC |= (0b11);
#endif
}

void PressMediaButton(MediaButton button)
{
	LogText("Out Media: ");
	LogIntLn(button);
	
#ifdef LOG
	if (button == MediaButtonVolumeDown)
	{
		MediaPort &= ~(MediaPortMask);
		PORTC |= MediaButtonVolumeDown;
		return;
	}
	if (button == MediaButtonVolumeUp)
	{
		MediaPort &= ~(MediaPortMask);
		PORTC |= MediaButtonVolumeUp;
		return;
	}

	PORTC &= ~(MediaButtonVolumeDown | MediaButtonVolumeUp);
#endif

	uint8_t port = MediaPort;
	port &= ~(MediaPortMask);
	port |= button;
	MediaPort = port;
}

void ReleaseMediaButton()
{
	PressMediaButton(MediaButtonNone);
}

void PressTurnButton(TurnButton button)
{
	LogText("Out Turn: ");
	LogIntLn(button);
	
	uint8_t port = TurnPort;
	port &= ~(TurnPortMask);
	port |= button;
	TurnPort = port;
}

void ReleaseTurnButton()
{
	PressTurnButton(TurnButtonNone);
}

void PressCruiseButton(CruiseButton button)
{
	LogText("Out Cruise: ");
	LogIntLn(button);
	
	uint8_t port = CruisePort;
	port &= ~(CruisePortMask);
	port |= button;
	CruisePort = port;
}

void ReleaseCruiseButton()
{
	PressCruiseButton(CruiseButtonNone);
}