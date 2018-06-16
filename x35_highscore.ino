
uint16_t highscoreAmount;

void highscoreSet(uint16_t hi) {
  highscoreAmount = hi;
  EEPROM.put(0, highscoreAmount);
}


void highscoreLoad() {
  EEPROM.get(0, highscoreAmount);
  if (highscoreAmount == 0xffff) {
    highscoreSet(0);
  }
}

