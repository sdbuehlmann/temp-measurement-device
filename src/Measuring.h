#ifndef MEASURING_H
#define MEASURING_H

// includes
#include <Arduino.h>
#include <DHT.h>

#include "StateMachine.h"
#include "Platform.h"
#include "Gui.h"
#include "CacheService.h"
#include "Logger.h"

// defines
#define WAITING_TIME_BETWEEN_MEASUREMENTS 5000

#define WAIT_STATE 0
#define MEASURE_STATE 1

// variables
SerialLogger logger = SerialLogger(__FILE__);
//DoNothingLogger logger = DoNothingLogger();

struct MeasuringVariables {
  int manualMeasureButtonState;
};
class MeasuringManager : public IVariablesReader<MeasuringVariables> {
  public:
    MeasuringManager();
    
    int proceed() {
      return sm.proceed();
    }
    void begin() {
      sm.begin(this);
    }
    
    virtual MeasuringVariables* read() override {
      return &variables;
    }

  private:
    // variables
    StateMachine<MeasuringVariables> sm;
    MeasuringVariables variables = MeasuringVariables();

    // transitions
    static bool wait_to_measure_transition(MeasuringVariables* pVariables, StateInfo* pInfo) {
      return pInfo->isStateMinActiveFor(WAITING_TIME_BETWEEN_MEASUREMENTS);
    }
    static bool measure_to_wait_transition(MeasuringVariables* pVariables, StateInfo* pInfo) {
      return pInfo->isStateMinActiveFor(1000);
    }

    // actions
    static void measure_state_entry_action(MeasuringVariables* pVariables, StateInfo* pInfo) {
      logger.log("Entry measure state");
      

      unsigned long deltaT = millis();
      MeasurementValues measurementValues = Platform::instance().measure();
      deltaT = millis() - deltaT;
      
      Gui::instance().getMeasurementScene().setLine1("Status: ", "Measure");

      char result[8];
      sprintf(result, "%-.1f", measurementValues.temperature);
      Gui::instance().getMeasurementScene().setLine2("Temp: ", result);
      
      sprintf(result, "%-.1f", measurementValues.humidity);
      Gui::instance().getMeasurementScene().setLine3("Hum: ", result);

      sprintf(result,"%ld", deltaT);
      Gui::instance().getMeasurementScene().setLine4("Delta t: ", result);

      CacheValue temp = {measurementValues.temperature, measurementValues.humidity, millis(), false};
      CacheService::instance().getBuffer().in(temp);

      Gui::instance().getHistoryScene().update();
    }
    static void wait_state_entry_action(MeasuringVariables* pVariables, StateInfo* pInfo) {
      logger.log("Entry wait state");
      Gui::instance().getMeasurementScene().setLine1("Status: ", "Waiting");
    }
};

MeasuringManager::MeasuringManager() : sm(2, 2) {
  sm.addState(WAIT_STATE, &MeasuringManager::wait_state_entry_action, 0, 0);
  sm.addState(MEASURE_STATE, &MeasuringManager::measure_state_entry_action, 0, 0);

  sm.addTransition(WAIT_STATE, MEASURE_STATE, &MeasuringManager::wait_to_measure_transition);
  sm.addTransition(MEASURE_STATE, WAIT_STATE, &MeasuringManager::measure_to_wait_transition);
}

#endif
