#include <EEPROM.h>
#include "snd_fail.h"
#include "snd_btn.h"

// ffmpeg -i [input file] -acodec pcm_u8 -ac 1 -ar 8000 out.wav
// cut wav headers in hexfiend
// xxd -i out.wav > out.h

#define PIN_LED_1 4
#define PIN_LED_2 5
#define PIN_LED_3 6
#define PIN_LED_4 7

#define PIN_BTN_1 8
#define PIN_BTN_2 9
#define PIN_BTN_3 10
#define PIN_BTN_4 12

#define PIN_SPEAKER 11

#define PIN_DISP_DATA A0
#define PIN_DISP_LOAD A1
#define PIN_DISP_CLK A2
