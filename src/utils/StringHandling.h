#ifndef STRING_HANDLING_H
#define STRING_HANDLING_H

#include <cstdarg>
#include <iostream>

class CharManager {
 public:
  CharManager(char* pBuffer, int sizeBuffer) {
    this->pBuffer = pBuffer;
    this->capacityBuffer =
        sizeBuffer - 1;  // last space in buffer is always reserved for a 0
    this->pBuffer[sizeBuffer - 1] = 0;
    this->nrBuffered = 0;
  }

  enum Feedback { SUCCESSFUL, FAILED_BECAUSE_OVERFLOW };

  /**
   * Does merge the specified textes and stores them in the internal buffer.
   * Caution: Arguments needs to be pointers on zero-terminated char arrays!
   */
  Feedback merge(int nrFragments, ...) {
    va_list args;
    va_start(args, nrFragments);

    this->nrBuffered = 0; // clear

    for (int i = 0; i < nrFragments; i++) {
      if (append(va_arg(args, char*)) != CharManager::Feedback::SUCCESSFUL) {
        va_end(args);
        return CharManager::Feedback::FAILED_BECAUSE_OVERFLOW;
      }
    }
    va_end(args);
    return CharManager::Feedback::SUCCESSFUL;
  }

  /**
   * Does append the specified text to the existing text in the buffer.
   */
  Feedback append(char* pTxt) {
    for (int i = 0; pTxt[i] != 0; i++) {
      if (nrBuffered < capacityBuffer) {
        pBuffer[nrBuffered] = pTxt[i];
        nrBuffered++;
      } else {
        return CharManager::Feedback::FAILED_BECAUSE_OVERFLOW;
      }
    }

    // last element needs to be a 0
    pBuffer[nrBuffered] = 0;
    return CharManager::Feedback::SUCCESSFUL;
  }

 private:
  // variables
  char* pBuffer;
  int capacityBuffer;
  int nrBuffered;
};

#endif