#include "EdgeDetector.h"
#include <Arduino.h>

#define DEBUG_PRINT(x) Serial.println(x);

EdgeDetector::EdgeDetector() {
}

EdgeDetector::EdgeDetector(const int _pin, const int _nrDebounceCycles) {
  start(_pin, _nrDebounceCycles);
}

void EdgeDetector::start(const int _pin, const int _nrDebounceCycles) {
  pin = _pin;
  nrDebounceCycles = _nrDebounceCycles;
  lastPublishedState = LOW;
  lastPublishedEdge = NO_CHANGE;
  count = 0;

  pinMode(_pin, INPUT);
}

int EdgeDetector::proceed() {
  int currentState = digitalRead(pin);
  
    if(currentState == lastPublishedState) {
    // no change
    if(count > 0){
      count--;
    }
    lastPublishedEdge = NO_CHANGE;
    return NO_CHANGE;
  } else {
    // change
    count++;
    if(count > nrDebounceCycles) {
      // publish new state
      count = 0;
      lastPublishedState = currentState;
      if(currentState == HIGH){
        lastPublishedEdge = RISING_EDGE;
        return RISING_EDGE;
      }
      lastPublishedEdge = FALLING_EDGE;
      return FALLING_EDGE;
    }
  }
}

int EdgeDetector::getState() {
  return lastPublishedState;
}

int EdgeDetector::getLastEdge() {
  return lastPublishedEdge;
}

// ======= StatefullButton ======= 

StatefullButton::StatefullButton(const int _pin, const int _nrDebounceCycles, const int _nrOfStates) {
  nrOfStates = _nrOfStates;
  activeState = 0;
  edgeDetector.start(_pin, _nrDebounceCycles);
}

int StatefullButton::proceed() {
  if(edgeDetector.proceed() == RISING_EDGE) {
    // next state active
    activeState = (activeState + 1) % nrOfStates;
    return NEW_STATE;
  }
  return NO_CHANGE;
}

int StatefullButton::getState() {
  return activeState;
}
