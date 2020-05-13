#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <Arduino.h>

#define BUFFER_OVERFLOW 0
#define BUFFER_SUCCESS 1
#define BUFFER_EMPTY 2

template <class T>
class Iterator {
  public:
    Iterator(const int _capacity) {
      capacity = _capacity;
      pArray = (T*)malloc(capacity * sizeof(T));
    }

    bool hasNext() {
      return idxNext < elementCount;
    }
    T* getNext() {
      T* temp = pArray + idxNext;
      idxNext++;
      return temp;
    }

    void reset() {
      elementCount = 0;
      idxNext = 0;
    }

    void add(T* pElement) {
      *(pArray + elementCount) = *pElement;
      elementCount++;
    }

    int getCapacity() {
      return capacity;
    }

    int getNrElements() {
      return elementCount;
    }

    int getIndexCurrentElement() {
      return idxNext - 1;
    }

  private:
    T* pArray;
    int capacity;
    int elementCount;
    int idxNext;
};

template <class T>
class RingBuffer {
  public:
    RingBuffer(const int _capacity) {
      capacity = _capacity;

      elementCount = 0;
      idxNextIn = 0;
      idxNextOut = 0;

      pArray = (T*)malloc(capacity * sizeof(T));
    }

    /**
       Does store the element in the buffer.
       Returns:
        BUFFER_SUCCESS: Element stored in empty space
        BUFFER_OVERFLOW: Element stored and overriden old element
    */
    int in(T element);

    void oldestToNewest(Iterator<T>& iterator) {
      iterator.reset();
      for (int i = 0; i < elementCount; i++) {
        int indexNext = (idxNextOut + i) % capacity;
        iterator.add(pArray + indexNext);
      }
    }

    void newestToOldest(Iterator<T>& iterator) {
      iterator.reset();
      for (int i = elementCount - 1; i >= 0; i--) {
        int indexNext = (idxNextOut + i) % capacity;
        iterator.add(pArray + indexNext);
      }
    }

    /*int out(T* pElement) {
      Serial.println("Out() call!!!!!!!!!!!!!!!!!!!!!!!!!!!");

      if(elementCount == 0) {
        return BUFFER_EMPTY;
      }

      T *pNextOut = pArray + idxNextOut;
       pElement = *pNextOut;
      idxNextOut = (idxNextOut + 1) % capacity;
      elementCount--;

      return BUFFER_SUCCESS;
      }*/

    int drop();

    // getter
    int getElementCount() {
      return elementCount;
    }

    int getCapacity() {
      return capacity;
    }

  private:
    T* pArray;
    unsigned int idxNextIn;
    unsigned int idxNextOut;

    int capacity;
    int elementCount;
};

template <class T>
int RingBuffer<T>::in(T element) {
  int feedback = BUFFER_SUCCESS;
  if (elementCount == capacity) {
    // override old element
    drop();
    feedback = BUFFER_OVERFLOW;
  }

  T *pNextIn = pArray + idxNextIn;
  *pNextIn = element; // copy

  idxNextIn = (idxNextIn + 1) % capacity;
  elementCount++;

  return feedback;
}

template <class T>
int RingBuffer<T>::drop() {
  if (elementCount == 0) {
    return BUFFER_EMPTY;
  }

  idxNextOut = (idxNextOut + 1) % capacity;
  elementCount--;

  return BUFFER_SUCCESS;
}

#endif
