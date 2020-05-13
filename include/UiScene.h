#ifndef UISCENE_H
#define UISCENE_H


#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "CacheService.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define MAX_NR_CHARS 21 // max 21
#define HEIGH_OF_LINE 8 // min 7
#define POS_FIRST_LINE 11 // min 11

#define SCENE_HEIGHT 32
#define SCENE_WIDTH SCREEN_WIDTH
#define SCENE_POS_TOP 11
#define SCENE_POS_BOTTOM SCENE_POS_TOP + SCENE_HEIGHT



class IUiScene {
  public:
    virtual void display() = 0;
    virtual char* getTitle() = 0; 
    virtual bool hasUndisplayedChanges() = 0;
    virtual ~IUiScene() { }
};

class LineBasedUiScene : public IUiScene {
  public:
    LineBasedUiScene(Adafruit_SSD1306* pDisplay, char title[]);

    void setLine1(char prefix[], char txt[]);
    void setLine2(char prefix[], char txt[]);
    void setLine3(char prefix[], char txt[]);
    void setLine4(char prefix[], char txt[]);

    virtual void display() override;
    char* getTitle() override;
    bool hasUndisplayedChanges() override;
  
  private:
    // variables
    Adafruit_SSD1306* pDisplay;

    bool changed = true;
    
    char title[MAX_NR_CHARS];

    char line1[MAX_NR_CHARS];
    char line2[MAX_NR_CHARS];
    char line3[MAX_NR_CHARS];
    char line4[MAX_NR_CHARS];

    // methods
    void setLine(char txt[], char buffer[]);
    void setLine(char prefix[], char txt[], char buffer[]); // ToDo: Method with variable number of char arrays (merged together)
};

class GraphUiScene : public IUiScene {
  public:
    GraphUiScene(Adafruit_SSD1306* pDisplay, char title[]);

    virtual void display() override;
    char* getTitle() override;
    bool hasUndisplayedChanges() override;

    void update();
  
  private:
    // variables
    Adafruit_SSD1306* pDisplay;
    //Iterator<CacheValue> iterator;

    bool changed = true;

    char title[MAX_NR_CHARS];

    void setLine(char txt[], char buffer[]);
};

#endif
