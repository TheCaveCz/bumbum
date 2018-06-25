
void setup() {
  randomSeed(analogRead(0));

  dispSetup();
  playbackSetup();
  ioSetup();

  highscoreLoad();

  uint8_t scan = ioScan(5);
  if (scan != 0) {
    dispSetImage(imgError);

    uint8_t b = 0;
    while ((scan & 1) == 0) {
      b++;
      scan >>= 1;
    }
    
    dispShowDigit(b, 3);
    dispUpdate();
    while (1);
  }

  modeSet(&modeReady);
}

void loop() {
  modeCurrent();
}

