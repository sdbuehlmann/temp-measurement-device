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
      Gui::instance().getMeasurementScene().setLine4("...", "");

      MeasurementValues measurementValues = Platform::instance().measure();
      CacheService::instance().getBuffer().in({measurementValues.temperature, measurementValues.humidity, pInfo->stateEnteredTime, false});
      MinMaxValues minMax = CacheService::instance().getMinMax();
      
      // update measure gui
      char result[30];
      sprintf(result, "%-.1f (%-.1f/%-.1f)", measurementValues.temperature, minMax.minTemperature, minMax.maxTemperature);
      Gui::instance().getMeasurementScene().getLine1().merge(2, "T: ", result);
      
      sprintf(result, "%-.1f (%-.1f/%-.1f)", measurementValues.humidity, minMax.minHumidity, minMax.maxHumidity);
      Gui::instance().getMeasurementScene().getLine2().merge(2, "H: ", result);

      // update history gui (graph)
      Gui::instance().getHistoryScene().update();
    }
    static void wait_state_entry_action(MeasuringVariables* pVariables, StateInfo* pInfo) {
      logger.log("Entry wait state");

      char result[30];
      unsigned int cachedTimeInSec = CacheService::instance().getCachedTimespan() / 1000;
      sprintf(result, "(Min/Max last %ds)", cachedTimeInSec);
      Gui::instance().getMeasurementScene().setLine4(result, "");
    }
};

MeasuringManager::MeasuringManager() : sm(2, 2) {
  sm.addState(WAIT_STATE, &MeasuringManager::wait_state_entry_action, 0, 0);
  sm.addState(MEASURE_STATE, &MeasuringManager::measure_state_entry_action, 0, 0);

  sm.addTransition(WAIT_STATE, MEASURE_STATE, &MeasuringManager::wait_to_measure_transition);
  sm.addTransition(MEASURE_STATE, WAIT_STATE, &MeasuringManager::measure_to_wait_transition);
}

#endif
