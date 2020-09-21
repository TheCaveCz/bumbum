
uint16_t mScore;
void (*modeCurrent)();


// initial mode, just shows image and highscore and goes to countdown
void modeReady() {
  dispSetImage(imgStart);
  while (ioScan(1) != 0) {} // wait a bit for button release after fail mode
  delay(500);


  while (true) {
    dispShowNumber(highscoreAmount);
    if (modeBlinkAndWait(1, 50, 50, &modeCountdown)) return;
    dispSetImage(imgStart);
    if (modeBlinkAndWait(1, 50, 50, &modeCountdown)) return;
  }
}

// 3 second countdown with sound and images, then goes to buttonWait mode
void modeCountdown() {
  mScore = 0;

  uint8_t resetStage = 0;

  ioLedMask(15);
  dispSetImage(imgCountdown3);
  playbackStart(sndBtn, sizeof(sndBtn));
  delay(1000);

  if (ioScan(1) == 5) resetStage++;

  ioLedMask(6);
  dispSetImage(imgCountdown2);
  playbackStart(sndBtn, sizeof(sndBtn));
  delay(1000);

  if (ioScan(1) == 10) resetStage++;

  ioLedMask(0);
  dispSetImage(imgCountdown1);
  playbackStart(sndBtn, sizeof(sndBtn));
  delay(1000);

  if (ioScan(1) == 5) resetStage++;

  if (resetStage < 3) {
    playbackStart(sndBtn, sizeof(sndBtn));
    modeSet(&modeButtonWait);
  } else {
    modeSet(&modeReset);
  }
}

uint8_t modeCalcBits(uint8_t ratio) {
  switch (ratio) {
    case 0: return 0b00000000;
    case 1: return 0b10000000;
    case 2: return 0b11000000;
    case 3: return 0b11100000;
    case 4: return 0b11110000;
    case 5: return 0b11111000;
    case 6: return 0b11111100;
    case 7: return 0b11111110;
    default: return 0b11111111;
  }
}

// picks one button at random, lights up the correct led, shows hint image and waits for pressing this button
// wait time lowers with score, goes to buttonOk or fail modes
void modeButtonWait() {
  uint8_t btn = random(4);

  ioLedMask(1 << btn);
  switch (btn) {
    case 0: dispSetImage(imgBtn1); break;
    case 1: dispSetImage(imgBtn2); break;
    case 2: dispSetImage(imgBtn3); break;
    case 3: dispSetImage(imgBtn4); break;
  }

  uint16_t t = 100 - min(90, mScore * 2 / 3);
  uint16_t tNow = t;
  uint8_t b1;
  do {
    uint8_t ratio = map(tNow, 0, t, 0, 31);
    dispFramebuffer[0] = modeCalcBits(ratio);
    dispFramebuffer[1] = ratio < 8 ? 0 : modeCalcBits(ratio - 8);
    dispFramebuffer[2] = ratio < 16 ? 0 : modeCalcBits(ratio - 16);
    dispFramebuffer[3] = ratio < 24 ? 0 : modeCalcBits(ratio - 24);
    dispUpdate();

    b1 = ioScanOnce();
    if (b1 == 1 << btn) {
      modeSet(&modeButtonOk);
      return;
    } else if (b1 != 0) {
      modeSet(&modeFail);
      return;
    }
    delay(2); // adjusted for display update rate
  } while (tNow--);

  modeSet(&modeFail);
}

// plays sound, shows highscore and then goes to buttonWait again
void modeButtonOk() {
  playbackStart(sndBtn, sizeof(sndBtn));
  ioLedMask(0);

  mScore++;
  dispShowNumber(mScore);

  delay(1000);
  modeSet(&modeButtonWait);
}

// plays sound, checks highscore and mocks the player. goes to ready
void modeFail() {
  playbackStart(sndFail, sizeof(sndFail));
  bool isHi = mScore > highscoreAmount;
  if (isHi) {
    dispSetImage(imgHiscore);
    highscoreSet(mScore);
  } else {
    dispSetImage(imgFail);
  }

  while (ioScan(1) != 0) {}
  delay(1500); // wait some time for button release

  uint8_t cnt = 10;
  while (cnt--) {
    dispShowNumber(mScore);
    if (modeBlinkAndWait(3, 20, 10, &modeCountdown)) return;
    if (isHi) {
      dispSetImage(imgHiscore);
    } else {
      dispSetImage(imgFail);
    }
    if (modeBlinkAndWait(3, 20, 10, &modeCountdown)) return;
  }

  modeSet(&modeReady);
}

void modeReset() {
  highscoreSet(0);
  dispSetImage(imgReset);
  while (ioScan(1) != 0) {} // wait a bit for button release after fail mode
  delay(2000);
  modeSet(&modeReady);
}



void modeSet(void (*cb)()) {
  modeCurrent = cb;
}

uint8_t modeBlinkAndWait(uint8_t repeats, uint8_t onCycles, uint8_t offCycles, void (*mode)()) {
  while (repeats--) {
    ioLedMask(15);
    if (ioScan(onCycles)) {
      modeSet(mode);
      return 1;
    }
    ioLedMask(0);
    if (ioScan(offCycles)) {
      modeSet(mode);
      return 1;
    }
  }
  return 0;
}
