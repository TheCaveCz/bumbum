
void ioSetup() {
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);
  pinMode(PIN_BTN_3, INPUT_PULLUP);
  pinMode(PIN_BTN_4, INPUT_PULLUP);

  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_LED_4, OUTPUT);

  ioLedMask(0);
}

uint8_t ioScanOnce() {
  return digitalRead(PIN_BTN_1) |
         (digitalRead(PIN_BTN_2) << 1) |
         (digitalRead(PIN_BTN_3) << 2) |
         (digitalRead(PIN_BTN_4) << 3);
}

uint8_t ioScan(const uint16_t cycles) {
  uint16_t cyclesRemaining = cycles;
  uint8_t b1;
  do {
    b1 = digitalRead(PIN_BTN_1) |
         (digitalRead(PIN_BTN_2) << 1) |
         (digitalRead(PIN_BTN_3) << 2) |
         (digitalRead(PIN_BTN_4) << 3);
    if (b1) break;
    delay(10);
  } while (cyclesRemaining--);

  return b1;
}

void ioLedMask(const uint8_t mask) {
  digitalWrite(PIN_LED_1, mask & 1);
  digitalWrite(PIN_LED_2, mask & 2);
  digitalWrite(PIN_LED_3, mask & 4);
  digitalWrite(PIN_LED_4, mask & 8);
}
