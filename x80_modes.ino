
uint16_t mScore;
void (*modeCurrent)();


// initial mode, just shows image and highscore and goes to countdown
void modeReady() {
  dispSetImage(imgStart);
  delay(1000); // wait a bit for button release after fail mode

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
  ioLedMask(15);

  dispSetImage(imgCountdown3);
  playbackStart(sndBtn, sizeof(sndBtn));
  delay(1000);

  dispSetImage(imgCountdown2);
  playbackStart(sndBtn, sizeof(sndBtn));
  delay(1000);

  dispSetImage(imgCountdown1);
  playbackStart(sndBtn, sizeof(sndBtn));
  delay(1000);

  playbackStart(sndBtn, sizeof(sndBtn));
  modeSet(&modeButtonWait);
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

  uint16_t t = 100 - min(90, mScore / 2);

  if (ioScan(t) == 1 << btn) {
    modeSet(&modeButtonOk);
  } else {
    modeSet(&modeFail);
  }
}

// plays sound, shows highscore and then goes to buttonWait again
void modeButtonOk() {
  playbackStart(sndBtn, sizeof(sndBtn));
  ioLedMask(15);

  mScore++;
  dispShowNumber(mScore);

  delay(1000);
  modeSet(&modeButtonWait);
}

// plays sound, checks highscore and mocks the player. goes to ready 
void modeFail() {
  playbackStart(sndFail, sizeof(sndFail));

  if (mScore > highscoreAmount) {
    dispSetImage(imgHiscore);
    highscoreSet(mScore);
  } else {
    dispSetImage(imgFail);
  }

  delay(3000); // wait some time for button release

  uint8_t cnt = 10;
  while (cnt--) {
    dispShowNumber(mScore);
    if (modeBlinkAndWait(3, 20, 10, &modeReady)) return;
    dispSetImage(imgFail);
    if (modeBlinkAndWait(3, 20, 10, &modeReady)) return;
  }

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