

uint8_t dispFramebuffer[32];

void dispSetup() {
  memset(dispFramebuffer, 0, sizeof(dispFramebuffer));

  pinMode(PIN_DISP_DATA, OUTPUT);
  pinMode(PIN_DISP_CLK, OUTPUT);
  pinMode(PIN_DISP_LOAD, OUTPUT);
  digitalWrite(PIN_DISP_LOAD, HIGH);

  dispSendAll(15, 0); // disable display test mode
  dispSendAll(11, 7); // set maximum scanlimit (8 rows)
  dispSendAll(9, 0); // disable BCD decoding (only for 7-seg displays)
  dispSetBrightness(8); // set initial brightness
  dispSendAll(12, 1); // enable display
}

void dispSetBrightness(int8_t val) {
  dispSendAll(10, constrain(val, 0, 15));
}

void dispSetImage(const uint8_t *data) {
  memcpy_P(dispFramebuffer, data, sizeof(dispFramebuffer));
  dispUpdate();
}

void dispSendAll(uint8_t command, uint8_t data) {
  digitalWrite(PIN_DISP_LOAD, LOW);
  shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, command);
  shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, data);
  shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, command);
  shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, data);
  shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, command);
  shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, data);
  shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, command);
  shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, data);
  digitalWrite(PIN_DISP_LOAD, HIGH);
}

void dispUpdate() {
  uint8_t fbPos = 28;
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(PIN_DISP_LOAD, LOW);
    shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, i + 1);
    shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, LSBFIRST, dispFramebuffer[fbPos + 3]);
    shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, i + 1);
    shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, LSBFIRST, dispFramebuffer[fbPos + 2]);
    shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, i + 1);
    shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, LSBFIRST, dispFramebuffer[fbPos + 1]);
    shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, MSBFIRST, i + 1);
    shiftOut(PIN_DISP_DATA, PIN_DISP_CLK, LSBFIRST, dispFramebuffer[fbPos]);
    digitalWrite(PIN_DISP_LOAD, HIGH);
    fbPos -= 4;
  }
}

void dispShowDigit(const uint8_t num, const uint8_t pos) {
  for (uint8_t i = 0; i < 8; i++) {
    dispFramebuffer[pos + (i << 2)] = pgm_read_byte(numbers + (num << 3) + i);
  }
}

void dispShowNumber(const uint16_t num) {
  uint16_t n = num;
  memset(dispFramebuffer, 0, sizeof(dispFramebuffer));
  dispShowDigit(n % 10, 3);
  n /= 10;
  if (n) {
    dispShowDigit(n % 10, 2);
    n /= 10;
    if (n) {
      dispShowDigit(n % 10, 1);
      n /= 10;
      if (n) {
        dispShowDigit(n % 10, 0);
      }
    }
  }
  dispUpdate();
}

