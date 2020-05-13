#include "UiScene.h"

LineBasedUiScene::LineBasedUiScene(Adafruit_SSD1306* pDisplay, char title[]) {
  this->pDisplay = pDisplay;
  setLine(title, this->title);
}

void LineBasedUiScene::setLine1(char prefix[], char txt[]) {
  setLine(prefix, txt, line1);
  changed = true;
}
void LineBasedUiScene::setLine2(char prefix[], char txt[]) {
  setLine(prefix, txt, line2);
  changed = true;
}
void LineBasedUiScene::setLine3(char prefix[], char txt[]) {
  setLine(prefix, txt, line3);
  changed = true;
}
void LineBasedUiScene::setLine4(char prefix[], char txt[]) {
  setLine(prefix, txt, line4);
  changed = true;
}

void LineBasedUiScene::display() {
  changed = false;

  pDisplay->setTextColor(WHITE);

  pDisplay->setCursor(0, POS_FIRST_LINE);
  pDisplay->print(line1);

  pDisplay->setCursor(0, POS_FIRST_LINE + HEIGH_OF_LINE);
  pDisplay->print(line2);

  pDisplay->setCursor(0, POS_FIRST_LINE + 2 * HEIGH_OF_LINE);
  pDisplay->print(line3);

  pDisplay->setCursor(0, POS_FIRST_LINE + 3 * HEIGH_OF_LINE);
  pDisplay->print(line4);
}

char* LineBasedUiScene::getTitle() { return title; }

bool LineBasedUiScene::hasUndisplayedChanges() { return changed; }

// private
void LineBasedUiScene::setLine(char txt[], char buffer[]) {
  for (int i = 0; i < MAX_NR_CHARS; i++) {
    if (txt[i] != 0) {
      buffer[i] = txt[i];
    } else {
      buffer[i] = 0;  // mark end
      break;
    }
  }
}

void LineBasedUiScene::setLine(char prefix[], char txt[], char buffer[]) {
  int lengthPrefix = 0;

  // add prefix
  for (int i = 0; i < MAX_NR_CHARS; i++) {
    if (prefix[i] != 0) {
      buffer[i] = prefix[i];
      lengthPrefix++;
    } else {
      break;
    }
  }

  // add text
  for (int i = lengthPrefix; i < MAX_NR_CHARS; i++) {
    if (txt[i - lengthPrefix] != 0) {
      buffer[i] = txt[i - lengthPrefix];
    } else {
      buffer[i] = 0;  // mark end
      break;
    }
  }
}