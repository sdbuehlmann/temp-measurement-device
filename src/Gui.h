#ifndef GUI_H
#define GUI_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

#include "Platform.h"
#include "StateMachine.h"
#include "UiButton.h"
#include "UiScene.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// states
#define STARTUP_DISPLAYED_STATE 0
#define MEASUREMENT_DISPLAYED_STATE 1
#define HISTORY_DISPLAYED_STATE 2
#define SETTINGS_DISPLAYED_STATE 3

struct ButtonsState {
  int button1;
  int button2;
  int button3;
};

class Gui : public IVariablesReader<ButtonsState> {
 public:
  static Gui& instance() {
    static Gui _instance;
    return _instance;
  }

  virtual ButtonsState* read() override {
    variables.button1 = Platform::instance().readButton1();
    variables.button2 = Platform::instance().readButton2();
    variables.button3 = Platform::instance().readButton3();

    return &variables;
  }

  void proceed() {
    sm.proceed();

    buttonBinding1.proceed();
    buttonBinding2.proceed();
    buttonBinding3.proceed();
  }

  void begin() {
    sm.begin(this);

    buttonBinding1.begin();
    buttonBinding2.begin();
    buttonBinding3.begin();
  }

  // getter
  LineBasedUiScene& getNetworkScene() { return networkScene; }

  LineBasedUiScene& getMeasurementScene() { return measurementScene; }

  GraphUiScene& getHistoryScene() { return historyScene; }

  Adafruit_SSD1306& getDisplay() { return display; }

 private:
  Gui();
  class Gui& operator=(const class Gui&);  // singleton

  void updateDisplay();
  void updateDisplayIfChanged();
  void displayStartupScreen();
  void displayScene(unsigned int id) {
    if (id > 2) {
      Serial.println("Illegal scene id!");
      return;
    }

    Serial.println("Display scene");
    pActiveScene = ppScenes[id];

    updateDisplay();
  }

  // variables
  StateMachine<ButtonsState> sm;
  ButtonsState variables = ButtonsState();

  Adafruit_SSD1306 display =
      Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

  // buttons
  UiTextField uiButton1 = UiTextField(&display, 42, 14, 0, 50);
  UiHwButton buttonBinding1 = UiHwButton(uiButton1, 1);

  UiTextField uiButton2 = UiTextField(&display, 42, 14, 43, 50);
  UiHwButton buttonBinding2 = UiHwButton(uiButton2, 2);

  UiTextField uiButton3 = UiTextField(&display, 42, 14, 86, 50);
  UiHwButton buttonBinding3 = UiHwButton(uiButton3, 3);

  // scenes
  LineBasedUiScene measurementScene = LineBasedUiScene(&display, "Measurement");
  GraphUiScene historyScene = GraphUiScene(&display, "History");
  LineBasedUiScene networkScene = LineBasedUiScene(&display, "Network");

  IUiScene* ppScenes[3] = {&measurementScene, &historyScene, &networkScene};
  IUiScene* pActiveScene = 0;

  // transitions
  static bool startup_to_measurement_transition(ButtonsState* pVariables,
                                                StateInfo* pInfo) {
    return pInfo->isStateMinActiveFor(5000);
  }

  static bool forward_sequential_transition(ButtonsState* pVariables,
                                            StateInfo* pInfo) {
    return pVariables->button3 == BUTTON_SWITCHED_ACTIVE;
  }

  static bool backward_sequential_transition(ButtonsState* pVariables,
                                             StateInfo* pInfo) {
    return pVariables->button1 == BUTTON_SWITCHED_ACTIVE;
  }

  // actions
  static void startup_entry_action(ButtonsState* pVariables, StateInfo* pInfo) {
    Gui::instance().displayStartupScreen();
  }
  static void state_entry_action(ButtonsState* pVariables, StateInfo* pInfo) {
    Gui::instance().displayScene(pInfo->activeStateId - 1);
  }
  static void state_proceed_action(ButtonsState* pVariables, StateInfo* pInfo) {
    Gui::instance().updateDisplayIfChanged();
  }
};

Gui::Gui() : sm(4, 7) {
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // ToDo: exception handling
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  sm.addState(STARTUP_DISPLAYED_STATE, Gui::startup_entry_action, 0, 0);
  sm.addState(MEASUREMENT_DISPLAYED_STATE, Gui::state_entry_action,
              Gui::state_proceed_action, 0);
  sm.addState(HISTORY_DISPLAYED_STATE, Gui::state_entry_action,
              Gui::state_proceed_action, 0);
  sm.addState(SETTINGS_DISPLAYED_STATE, Gui::state_entry_action,
              Gui::state_proceed_action, 0);

  sm.addTransition(STARTUP_DISPLAYED_STATE, MEASUREMENT_DISPLAYED_STATE,
                   &Gui::startup_to_measurement_transition);

  sm.addTransition(MEASUREMENT_DISPLAYED_STATE, HISTORY_DISPLAYED_STATE,
                   &Gui::forward_sequential_transition);
  sm.addTransition(HISTORY_DISPLAYED_STATE, MEASUREMENT_DISPLAYED_STATE,
                   &Gui::backward_sequential_transition);

  sm.addTransition(HISTORY_DISPLAYED_STATE, SETTINGS_DISPLAYED_STATE,
                   &Gui::forward_sequential_transition);
  sm.addTransition(SETTINGS_DISPLAYED_STATE, HISTORY_DISPLAYED_STATE,
                   &Gui::backward_sequential_transition);

  sm.addTransition(SETTINGS_DISPLAYED_STATE, MEASUREMENT_DISPLAYED_STATE,
                   &Gui::forward_sequential_transition);
  sm.addTransition(MEASUREMENT_DISPLAYED_STATE, SETTINGS_DISPLAYED_STATE,
                   &Gui::backward_sequential_transition);

  uiButton1.setText("<");
  uiButton2.setText("OK");
  uiButton3.setText(">");
}

void Gui::updateDisplayIfChanged() {
  // do only update, if anything changed
  if (pActiveScene->hasUndisplayedChanges() ||
      uiButton1.hasUndisplayedChanges() || 
      uiButton2.hasUndisplayedChanges() ||
      uiButton3.hasUndisplayedChanges()) {
    updateDisplay();
  }
}

void Gui::updateDisplay() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print(pActiveScene->getTitle());
  display.drawLine(0, 9, SCREEN_WIDTH, 9, WHITE);

  pActiveScene->display();

  uiButton1.paint();
  uiButton2.paint();
  uiButton3.paint();

  display.display();
}

void Gui::displayStartupScreen() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);

  display.setCursor(45, 10);
  display.print("Hi!");
  display.display();
}

#endif
