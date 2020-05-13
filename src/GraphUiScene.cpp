// includes
#include "UiScene.h"
#include "Logger.h"

// defines
#define Y_AXIS_MIN 0
#define Y_AXIS_MAX 30
#define Y_AXIS_RANGE Y_AXIS_MAX - Y_AXIS_MIN
#define Y_AXIS_SCALE SCENE_HEIGHT/Y_AXIS_RANGE

// variables
SerialLogger uiLogger = SerialLogger(__FILE__);
//DoNothingLogger uiLogger = DoNothingLogger();

GraphUiScene::GraphUiScene(Adafruit_SSD1306* pDisplay, char title[]) {
  this->pDisplay = pDisplay;
  setLine(title, this->title);
}

void GraphUiScene::display() {
  uiLogger.log("Draw graph");
  changed = false;

  Iterator<CacheValue> iterator = CacheService::instance().getIterator();
  while (iterator.hasNext()) {
    CacheValue* pValue = iterator.getNext();

    float scaledTemperature = pValue->temperature * Y_AXIS_SCALE;
    int positionDot = SCENE_POS_BOTTOM - scaledTemperature;

    pDisplay->drawPixel(iterator.getIndexCurrentElement(), positionDot, WHITE);
  }
  /*while(iterator.hasNext()) {
      CacheValue* pValue = iterator.getNext();

      pDisplay->drawPixel(iterator.getIndexCurrentElement(),
  pValue->temperature, WHITE);
      //pDisplay->drawBitmap()

    }
  }*/
}

char* GraphUiScene::getTitle() { return this->title; }
bool GraphUiScene::hasUndisplayedChanges() { return this->changed; }

void GraphUiScene::update() { this->changed = true; }

void GraphUiScene::setLine(char txt[],
                           char buffer[]) {  // ToDo: Remove duplicated code
  for (int i = 0; i < MAX_NR_CHARS; i++) {
    if (txt[i] != 0) {
      buffer[i] = txt[i];
    } else {
      buffer[i] = 0;  // mark end
      break;
    }
  }
}