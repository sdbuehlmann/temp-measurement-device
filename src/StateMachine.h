#ifndef STATEMACHINE_H
#define STATEMACHINE_H



#include <Arduino.h>
#include "EdgeDetector.h"

#define STATE_MACHINE_DEBUG // commend out to disable logging
#ifdef STATE_MACHINE_DEBUG
  #define DEBUG_PRINT(x) Serial.println(x);
#else
  #define DEBUG_PRINT(x)
#endif

#define NO_CHANGE 0
#define NEW_STATE 1

class StateInfo {
  public:
    int activeStateId;
    unsigned long stateEnteredTime;
    unsigned long stateActiveTime;

    bool isStateMinActiveFor(unsigned long timespan) {
      return stateActiveTime > timespan;
    }
};

template <class T>
class Transition {
  public:
    int fromState;
    int toState;
    bool (*transiate)(T*, StateInfo*);
};

template <class T>
class State {
  public:
    int id;
    void (*entry)(T*, StateInfo*);
    void (*proceed)(T*, StateInfo*);
    void (*exit)(T*, StateInfo*);
};

template <class T>
class IVariablesReader {
public:
  virtual T* read() = 0;
  virtual ~IVariablesReader() = 0;
};
template <class T>
inline IVariablesReader<T>::~IVariablesReader() { } // should be faster (source: https://isocpp.org/wiki/faq/pointers-to-members)

template <class T>
class StateMachine {
  public:
    StateMachine(const int nrStates, const int nrTransitions);

    void addState(int state, void (*entry)(T*, StateInfo*),  void (*proceed)(T*, StateInfo*),  void (*exit)(T*, StateInfo*));
    void addTransition(int fromState, int toState, bool (*transiate)(T*, StateInfo*));

    void begin(IVariablesReader<T>* _pReader);
      /**
       * Does proceed the statemachine.
       * Returns:
       *   NO_CHANGE: No state change
       *   NEW_STATE: State changed
       */
    int proceed();

    // getter
    StateInfo* getStateInfo();

  private:
    // variables
    ArrayList<State<T>> states;
    ArrayList<Transition<T>> transitions;
    IVariablesReader<T>* pReader;
    State<T>* pActiveState = 0;
    
    StateInfo stateInfo = StateInfo();

    int addTransitionCount = 0;
    int addStateCount = 0;

    // methods
    /**
     * Does handle the transitions. 
     * Returns:
     *   -1: No transition of the active state returns "true"
     *   ID: ID of the next active state (one of the transitions of the active state returns "true")
     */
    int handleTransitions(T* pVariables);

    /**
     * Does switch the active state and handles all entry/exit actions
     */
    void handleStates(int idNextState, T* pVariables);

    // helpers
    void callStateAction(void (*action)(T*, StateInfo*), T* pVariables) { // implizit inline https://www.geeksforgeeks.org/inline-functions-cpp/
      if(action != 0) {
        action(pVariables, &stateInfo);
      }
    }
};

template <class T>
StateMachine<T>::StateMachine(const int _nrStates, const int _nrTransitions)
  : states(_nrStates), transitions(_nrTransitions){ // member initializer list (source: https://stackoverflow.com/questions/12927169/how-can-i-initialize-c-object-member-variables-in-the-constructor)
  for (int i = 0; i < _nrStates; i++) {
    State<T>* pState = (State<T>*) malloc(sizeof(State<T>));
    states.add(pState);

    if(pActiveState == 0) {
      pActiveState = pState;
    }
  }
  
  for (int i = 0; i < _nrTransitions; i++) {
    Transition<T>* pTransition = (Transition<T>*) malloc(sizeof(Transition<T>));
    transitions.add(pTransition);
  }
}

template <class T>
void StateMachine<T>::addState(int id, void (*entry)(T*, StateInfo*),  void (*proceed)(T*, StateInfo*),  void (*exit)(T*, StateInfo*)) {
  State<T>* pState = states.get(addStateCount);
  pState->id = id;
  pState->entry = entry;
  pState->proceed = proceed;
  pState->exit = exit;
  
  addStateCount++;
}

template <class T>
void StateMachine<T>::addTransition(int fromState, int toState, bool (*transiate)(T*, StateInfo*)) {
  Transition<T>* pTransition = transitions.get(addTransitionCount);
  pTransition->fromState = fromState;
  pTransition->toState = toState;
  pTransition->transiate = transiate;
  
  addTransitionCount++;
}

template <class T>
void StateMachine<T>::begin(IVariablesReader<T>* _pReader) {
  pReader = _pReader;

  stateInfo.activeStateId = pActiveState->id;
  stateInfo.stateEnteredTime = millis();
  stateInfo.stateActiveTime = 0;
  
  T* pValues =  pReader->read();
  
  callStateAction(pActiveState->entry, pValues);
}

template <class T>
int StateMachine<T>::proceed() {
  T* pValues =  pReader->read();
  stateInfo.stateActiveTime = millis()- stateInfo.stateEnteredTime;

  int idNextState = handleTransitions(pValues);
  handleStates(idNextState, pValues);

  if(idNextState == -1){
    return NO_CHANGE;
  }
  return NEW_STATE;
}

template <class T>
int StateMachine<T>::handleTransitions(T* pVariables) {
  for (int i = 0; i < transitions.getSize(); i++) {
    Transition<T>* pTransition = transitions.get(i);
    if(pTransition->fromState == pActiveState->id 
    && pTransition->transiate != 0
    && pTransition->transiate(pVariables, &stateInfo)) {
      return pTransition->toState;
    }
  }
  return -1;
}

template <class T>
void StateMachine<T>::handleStates(int idNextState, T* pVariables) {
  if(idNextState != -1) {
    Serial.print("Change to state: ");
    Serial.println(idNextState);
    
    callStateAction(pActiveState->exit, pVariables);
    
    for (int i = 0; i < states.getSize(); i++) {
      State<T>* pState = states.get(i);
      
      if(pState->id == idNextState) {
        pActiveState = pState;

        stateInfo.activeStateId = pActiveState->id;
        stateInfo.stateEnteredTime = millis();
        stateInfo.stateActiveTime = 0;
        
        callStateAction(pActiveState->entry, pVariables);
        return;
      }
    }

    DEBUG_PRINT("FATAL: State not found");
  }
  
  callStateAction(pActiveState->proceed, pVariables);
}

// getters
template <class T>
StateInfo* StateMachine<T>::getStateInfo() {
  return &stateInfo;
}


#endif
