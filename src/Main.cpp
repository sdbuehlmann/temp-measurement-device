//Libraries
#include <Arduino.h>
#include <DHT.h>

#include "MainStateMachine.h"
#include "Measuring.h"
#include "Platform.h"

MainStateMachine mainSm;
MeasuringManager measuringManager;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.println("LOLIN32 playground started");

  mainSm = MainStateMachine();
  measuringManager = MeasuringManager();
  
  mainSm.begin();
  measuringManager.begin();

  Gui::instance().begin();
}

void loop() {
  Platform::instance().proceed();
  Gui::instance().proceed();
  
  mainSm.proceed();
  measuringManager.proceed();

  Platform::instance().switchLed(LED_ON);
}
