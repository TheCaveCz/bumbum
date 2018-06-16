/*
   speaker_pcm

   Plays 8-bit PCM audio on pin 11 using pulse-width modulation (PWM).
   For Arduino with Atmega168 at 16 MHz.

   Uses two timers. The first changes the sample value 8000 times a second.
   The second holds pin 11 high for 0-255 ticks out of a 256-tick cycle,
   depending on sample value. The second timer repeats 62500 times per second
   (16000000 / 256), much faster than the playback rate (8000 Hz), so
   it almost sounds halfway decent, just really quiet on a PC speaker.

   Takes over Timer 1 (16-bit) for the 8000 Hz timer. This breaks PWM
   (analogWrite()) for Arduino pins 9 and 10. Takes Timer 2 (8-bit)
   for the pulse width modulation, breaking PWM for pins 11 & 3.

   References:
       http://www.uchobby.com/index.php/2007/11/11/arduino-sound-part-1/
       http://www.atmel.com/dyn/resources/prod_documents/doc2542.pdf
       http://www.evilmadscientist.com/article.php/avrdac
       http://gonium.net/md/2006/12/27/i-will-think-before-i-code/
       http://fly.cc.fer.hr/GDM/articles/sndmus/speaker2.html
       http://www.gamedev.net/reference/articles/article442.asp

   Michael Smith <michael@hurts.ca>
*/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define SAMPLE_RATE 8000



unsigned char const *playbackData;
uint16_t playbackSampleLen;
volatile uint16_t playbackSamplePos;
byte playbackLastSample;
volatile byte playbackActive;


// This is called at 8000 Hz to load the next sample.
ISR(TIMER1_COMPA_vect) {
  if (playbackSamplePos >= playbackSampleLen) {
    if (playbackSamplePos == playbackSampleLen + playbackLastSample) {
      playbackStop();
    }
    else {
      // Ramp down to zero to reduce the click at the end of playback.
      OCR2A = playbackSampleLen + playbackLastSample - playbackSamplePos;
    }
  }
  else {
    OCR2A = pgm_read_byte(&playbackData[playbackSamplePos]);
  }

  ++playbackSamplePos;
}

void playbackSetup() {
  playbackActive = 0;
}

void playbackStart(unsigned char const *data, int length) {
  playbackData = data;
  playbackSampleLen = length;

  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.

  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);

  // Do non-inverting PWM on pin OC2A (p.155)
  // On the Arduino this is pin 11.
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
  TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));

  // No prescaler (p.158)
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

  // Set initial pulse width to the first sample.
  OCR2A = pgm_read_byte(&playbackData[0]);


  // Set up Timer 1 to send a sample every interrupt.

  cli();

  // Set CTC mode (Clear Timer on Compare Match) (p.133)
  // Have to set OCR1A *after*, otherwise it gets reset to 0!
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));

  // No prescaler (p.134)
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = F_CPU / SAMPLE_RATE;    // 16e6 / 8000 = 2000

  // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 |= _BV(OCIE1A);

  playbackLastSample = pgm_read_byte(&playbackData[playbackSampleLen - 1]);
  playbackSamplePos = 0;
  playbackActive = 1;
  pinMode(PIN_SPEAKER, OUTPUT);

  sei();
}

void playbackStop() {
  // Disable playback per-sample interrupt.
  TIMSK1 &= ~_BV(OCIE1A);

  // Disable the per-sample timer completely.
  TCCR1B &= ~_BV(CS10);

  // Disable the PWM timer.
  TCCR2B &= ~_BV(CS10);

  pinMode(PIN_SPEAKER, INPUT);

  playbackActive = 0;
}
