#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

#define NO_CHANGE 0

#define RISING_EDGE 1
#define FALLING_EDGE -1

#define NEW_STATE 1

class EdgeDetector {
  public:
      EdgeDetector();
      EdgeDetector(const int pin, const int nrDebounceCycles);
      
      void start(const int pin, const int nrDebounceCycles);

      /**
       * Returns HIGH or LOW
       */
      int getState();

      /**
       * Does return:
       *   NO_CHANGE: No change in logical signal in last proceed() call
       *   RISING_EDGE: Changed from low to high in last proceed() call
       *   FALLING_EDGE: Changed from high to low in last proceed() call
       */
       int getLastEdge();
  
      /**
       * Does read the pin and detect changes in the logical signal (debounced).
       * Does return:
       *   NO_CHANGE: No change in logical signal
       *   RISING_EDGE: Changed from low to high
       *   FALLING_EDGE: Changed from high to low
       */
      int proceed();
  
  private:
      int pin;
      int nrDebounceCycles;
      /**
       * HIGH or LOW
       */
      int lastPublishedState;
      int lastPublishedEdge;
      int count;
};

class StatefullButton {
  public:
      StatefullButton(const int pin, const int nrDebounceCycles, const int nrOfStates);
  
      /**
       * Does detect rising edges and proceed the state of the button.
       * Does return:
       *   NO_CHANGE: No change in logical signal
       *   NEW_STATE: New entered
       */
      int proceed();

      int getState();
  
  private:
      EdgeDetector edgeDetector = EdgeDetector();
      int activeState;
      int nrOfStates;
};

class Test { // ToDo: Remove
  public:
    Test(const int _myVar) { myVar = _myVar; }
    int myVar;
};

template <class T> class ArrayList {
  public:
    ArrayList(const int capacity);
    ArrayList(T* internalBuffer[], const int capacity);

    int add(T * pElement);
    T * get(int index);
    int getSize();

  private:
    T** ppFirstElement;
    int size;
    int capacity;
};

// ======= ArrayList ======= 


template <class T>
ArrayList<T>::ArrayList(const int _capacity) {
  capacity = _capacity;
  size = 0;
  ppFirstElement = (T**)malloc(_capacity * sizeof(T*));
}

template <class T>
ArrayList<T>::ArrayList(T* _internalBuffer[], const int _capacity) {
  capacity = _capacity;
  size = 0;
  ppFirstElement = _internalBuffer;
}

template <class T>
int ArrayList<T>::add(T* pElement) {
  *(ppFirstElement + size) = pElement;
  size++;

  return 0;
}

template <class T>
T * ArrayList<T>::get(int index) {
  return *(ppFirstElement + index);
}

template <class T>
int ArrayList<T>::getSize() {
  return size;
}

#endif
