
void setup() {
  randomSeed(analogRead(0));

  dispSetup();
  playbackSetup();
  ioSetup();

  highscoreLoad();

  modeSet(&modeReady);
}

void loop() {
  modeCurrent();
}

