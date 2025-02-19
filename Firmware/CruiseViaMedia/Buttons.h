#ifndef BUTTONS_H_
#define BUTTONS_H_

typedef enum ButtonSource {
	ButtonSourceMedia,
	ButtonSourceTurn,
	ButtonSourceCruise,
} ButtonSource;

typedef enum Button {
	ButtonNone = 0x00,
	ButtonError = 0xFF
} Button;

typedef enum MediaButton {
	MediaButtonNone = ButtonNone,
	MediaButtonVolumeDown = 0x01,
	MediaButtonVolumeUp = 0x02,
	MediaButtonSource = 0x04,
	MediaButtonNext = 0x08,
	MediaButtonPrevious = 0x10,
	MediaButtonArrowUp = 0x20,
	MediaButtonError = ButtonError,
} MediaButton;

typedef enum TurnButton {
	TurnButtonNone = ButtonNone,
	TurnButtonRight = 0x40,
	TurnButtonLeft = 0x80,
	TurnButtonError = ButtonError,
} TurnButton;

typedef enum CruiseButton {
	CruiseButtonNone = ButtonNone,
	CruiseButtonOff = 0x01,
	CruiseButtonResume = 0x02,
	CruiseButtonSet = 0x04,
	CruiseButtonError = ButtonError,
} CruiseButton;

#endif /* BUTTONS_H_ */