#include <Arduino.h>

#include "EdgeDetector.h"
#include "StateMachine.h"
#include "Platform.h"
#include "Gui.h"

// Uncomment to debug
#define MAINE_STATE_MACHINE_DEBUG

#ifdef MAINE_STATE_MACHINE_DEBUG
#define DEBUG_PRINT(x) Serial.println(x);
#else
#define DEBUG_PRINT(x)
#endif

#define DISCONNECTED_STATE 0
#define CONNECTING_STATE 1
#define CONNECTING_FAILED_STATE 2
#define CONNECTED_STATE 3
#define DISCONNECTING_STATE 4

#define MAX_WIFI_CONNECTING_TIME 10000

struct MainSmVariables {
  int buttonState;
  int wifiState;
};

class MainStateMachine : public IVariablesReader<MainSmVariables> {
  public:
    MainStateMachine();
    
    int proceed() {
      return sm.proceed();
    }
    void begin() {
      sm.begin(this);
    }
    
    virtual MainSmVariables* read() override {
      variables.buttonState = Platform::instance().readButton2();
      variables.wifiState = Platform::instance().getWifiStatus();
      
      return &variables;
    }

  private:
    // variables
    StateMachine<MainSmVariables> sm;
    MainSmVariables variables = MainSmVariables();

    // transitions
    static bool disconnected_to_connecting_transition(MainSmVariables* pVariables, StateInfo* pInfo) {
      return pVariables->buttonState == BUTTON_SWITCHED_ACTIVE;
    }
    static bool connecting_to_connected_transition(MainSmVariables* pVariables, StateInfo* pInfo) {
      return pVariables->wifiState == WIFI_CONNECTED;
    }
    static bool connected_to_disconnecting_transition(MainSmVariables* pVariables, StateInfo* pInfo) {
      return pVariables->buttonState == BUTTON_SWITCHED_ACTIVE;
    }
    static bool disconnecting_to_disconnected_transition(MainSmVariables* pVariables, StateInfo* pInfo) {
      return pVariables->wifiState == WIFI_DISCONNECTED && pInfo->isStateMinActiveFor(1000);
    }
    static bool connecting_to_failed_transition(MainSmVariables* pVariables, StateInfo* pInfo) {
      return pInfo->isStateMinActiveFor(MAX_WIFI_CONNECTING_TIME);
    }
    static bool failed_to_disconnected_transition(MainSmVariables* pVariables, StateInfo* pInfo) {
      return pVariables->buttonState == BUTTON_SWITCHED_ACTIVE;
    }

    // actions
    static void disconnected_state_entry_action(MainSmVariables* pVariables, StateInfo* pInfo) {
      DEBUG_PRINT("Entry disconnected state");
      updateGui("Disconnected");
    }
    static void connecting_state_entry_action(MainSmVariables* pVariables, StateInfo* pInfo) {
      DEBUG_PRINT("Entry connecting state");
      updateGui("Connecting");
      Platform::instance().connectWifi();
    }
    static void connected_state_entry_action(MainSmVariables* pVariables, StateInfo* pInfo) {
      DEBUG_PRINT("Entry connected state");
      updateGui("Connected");
    }
    static void disconnecting_state_entry_action(MainSmVariables* pVariables, StateInfo* pInfo) {
      DEBUG_PRINT("Entry disconnecting state");
      updateGui("Disconnecting");
      Platform::instance().disconnectWifi();
    }
    static void failed_state_entry_action(MainSmVariables* pVariables, StateInfo* pInfo) {
      DEBUG_PRINT("Entry failed state");
      updateGui("Error");
      Platform::instance().disconnectWifi();
    }

    // helpers
    static void updateGui(char* pStatus) {
      Gui::instance().getNetworkScene().setLine1("Status: ", pStatus);
      Gui::instance().getNetworkScene().setLine2("SSID: ", Platform::instance().getSsid());
      Gui::instance().getNetworkScene().setLine3("IP: ", Platform::instance().getIpAdress());
    }
};

MainStateMachine::MainStateMachine() : sm(5, 6) { 
  DEBUG_PRINT("Constructor call main state machine");
  sm.addState(DISCONNECTED_STATE, &MainStateMachine::disconnected_state_entry_action, 0, 0);
  sm.addState(CONNECTING_STATE, &MainStateMachine::connecting_state_entry_action, 0, 0);
  sm.addState(CONNECTING_FAILED_STATE, &MainStateMachine::failed_state_entry_action, 0, 0);
  sm.addState(CONNECTED_STATE, &MainStateMachine::connected_state_entry_action, 0, 0);
  sm.addState(DISCONNECTING_STATE, &MainStateMachine::disconnecting_state_entry_action, 0, 0);

  sm.addTransition(DISCONNECTED_STATE, CONNECTING_STATE, &MainStateMachine::disconnected_to_connecting_transition);
  sm.addTransition(CONNECTING_STATE, CONNECTING_FAILED_STATE, &MainStateMachine::connecting_to_failed_transition);
  sm.addTransition(CONNECTING_FAILED_STATE, DISCONNECTED_STATE, &MainStateMachine::failed_to_disconnected_transition);
  sm.addTransition(CONNECTING_STATE, CONNECTED_STATE, &MainStateMachine::connecting_to_connected_transition);
  sm.addTransition(CONNECTED_STATE, DISCONNECTING_STATE, &MainStateMachine::connected_to_disconnecting_transition);
  sm.addTransition(DISCONNECTING_STATE, DISCONNECTED_STATE, &MainStateMachine::disconnecting_to_disconnected_transition);
}
