#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H

#include <Arduino.h>
#include "RingBuffer.h"

#define CACHE_SIZE 128

struct CacheValue {
  float temperature;
  float humidity;
  unsigned long time;
  bool published;
};
class CacheService {
 public:
  static CacheService& instance() {
    static CacheService _instance;
    return _instance;
  }

  RingBuffer<CacheValue>& getBuffer() { return buffer; }

  Iterator<CacheValue>& getIterator() { 
    buffer.newestToOldest(iterator);
    return iterator; 
    }

 private:
  CacheService() {}
  class CacheService& operator=(const class CacheService&);  // singleton

  RingBuffer<CacheValue> buffer = RingBuffer<CacheValue>(CACHE_SIZE);
  Iterator<CacheValue> iterator = Iterator<CacheValue>(CACHE_SIZE);
};

#endif
