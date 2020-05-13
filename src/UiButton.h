#include <Adafruit_SSD1306.h>

#include "StateMachine.h"
#include "ui/UiTextField.h"
#include "Platform.h"

// defines
#define ENABLED_STATE 0
#define TIPPED_STATE 1

struct UiHwButtonVariables {
  int buttonState;
  UiTextField* pTextField;
};

class UiHwButton : public IVariablesReader<UiHwButtonVariables> {
  public:
    UiHwButton(UiTextField& textField, const int buttonId);

    int proceed() {
      sm.proceed();
    }

    void begin() {
      sm.begin(this);
    }

    virtual UiHwButtonVariables* read() override {
      variables.pTextField = &textField;
      variables.buttonState = Platform::instance().readButton(buttonId);

      return &variables;
    }

  private:
    int buttonId;
    UiTextField& textField;

    // variables
    StateMachine<UiHwButtonVariables> sm;
    UiHwButtonVariables variables = UiHwButtonVariables();

    // transitions
    static bool enabled_to_tipped_transition(UiHwButtonVariables* pVariables, StateInfo* pInfo) {
      return pVariables->buttonState == BUTTON_SWITCHED_ACTIVE;
    }
    static bool tipped_to_enabled_transition(UiHwButtonVariables* pVariables, StateInfo* pInfo) {
      return pInfo->isStateMinActiveFor(200);
    }

    // actions
    static void enabled_state_entry_action(UiHwButtonVariables* pVariables, StateInfo* pInfo) {
      pVariables->pTextField->setHighlighted(false);
    }
    static void tipped_state_entry_action(UiHwButtonVariables* pVariables, StateInfo* pInfo) {
      pVariables->pTextField->setHighlighted(true);
    }
};

UiHwButton::UiHwButton(UiTextField& _textField, const int _buttonId) : 
        buttonId(_buttonId),
        textField(_textField),
        sm(2, 2) {

    sm.addState(ENABLED_STATE, &UiHwButton::enabled_state_entry_action, 0, 0);
    sm.addState(TIPPED_STATE, &UiHwButton::tipped_state_entry_action, 0, 0);

    sm.addTransition(ENABLED_STATE, TIPPED_STATE, &UiHwButton::enabled_to_tipped_transition);
    sm.addTransition(TIPPED_STATE, ENABLED_STATE, &UiHwButton::tipped_to_enabled_transition);
}