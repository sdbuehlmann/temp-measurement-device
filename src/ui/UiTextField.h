#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define TEXT_HIGHT 7
#define CHAR_WIDTH 5
#define BETWEEN_CHAR_WIDTH 1

#define MAX_TEXT_LENGTH 10

class UiTextField {
  public:
    UiTextField(Adafruit_SSD1306* pDisplay, const int width, const int height, const int xPos, const int yPos) {
        this->pDisplay = pDisplay;
        this->width = width;
        this->height = height;
        this->xPos = xPos;
        this->yPos = yPos;

        this->xPosText = 0; // needs to be calculated, if the button text is set
        this->yPosText = yPos + ((height - TEXT_HIGHT) / 2);

        this->highlighted = false;
    }

    void setText(char text[]) {
        int textLength = 0;

        for(int i = 0; i < MAX_TEXT_LENGTH; i++) {
            this->text[i] = text[i];

            if(text[i] != 0) {
                textLength+=CHAR_WIDTH;
                textLength+=BETWEEN_CHAR_WIDTH;
            } else {
                textLength-=BETWEEN_CHAR_WIDTH; // last loop cycle handled the last char -> no space needed
                break;
            }
        }

        xPosText = xPos + ((width - textLength) / 2);
    }

    void setHighlighted(bool highlighted) {
        changed = true;
        this->highlighted = highlighted;
    }

    bool hasUndisplayedChanges() { return this->changed; }

    void paint() {
        changed = false;

        if(highlighted) {
            pDisplay->setTextColor(BLACK);
            pDisplay->fillRect(xPos, yPos, width, height, WHITE);
        } else {
            pDisplay->setTextColor(WHITE);
            pDisplay->drawRect(xPos, yPos, width, height, WHITE);
        }

        pDisplay->setCursor(xPosText, yPosText);
        pDisplay->print(text);
    }

  private:
    int width;
    int height;
    int xPos;
    int yPos;

    int xPosText;
    int yPosText;
    char text[10];

    bool highlighted;
    bool changed;

    Adafruit_SSD1306* pDisplay;
};