#ifndef BUFFERSERVICE_H
#define BUFFERSERVICE_H

#include <Arduino.h>
#include <limits>
#include "RingBuffer.h"

#define CACHE_SIZE 128

struct CacheValue {
  float temperature;
  float humidity;
  unsigned long time;
  bool published;
};
struct MinMaxValues {
  float minTemperature;
  float maxTemperature;
  float minHumidity;
  float maxHumidity;
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

  MinMaxValues getMinMax() {
    MinMaxValues values;

    values.maxTemperature = std::numeric_limits<float>::min();
    values.maxHumidity = std::numeric_limits<float>::min();

    values.minTemperature = std::numeric_limits<float>::max();
    values.minHumidity = std::numeric_limits<float>::max();

    buffer.newestToOldest(iterator);
    while(iterator.hasNext()) {
      CacheValue* pValue = iterator.getNext();

      // temp
      if(pValue->temperature > values.maxTemperature) {
        values.maxTemperature = pValue->temperature;
      }
      if(pValue->temperature < values.minTemperature) {
        values.minTemperature = pValue->temperature;
      }

      // hum
      if(pValue->humidity > values.maxHumidity) {
        values.maxHumidity = pValue->humidity;
      }
      if(pValue->humidity < values.minHumidity) {
        values.minHumidity = pValue->humidity;
      }
    }

    return values;
  }

  unsigned long getCachedTimespan() {
    unsigned long timespan = buffer.getNewest()->time;
    timespan = timespan - buffer.getOldest()->time;

    return timespan;
  }

 private:
  CacheService() {}
  class CacheService& operator=(const class CacheService&);  // singleton

  RingBuffer<CacheValue> buffer = RingBuffer<CacheValue>(CACHE_SIZE);
  Iterator<CacheValue> iterator = Iterator<CacheValue>(CACHE_SIZE);
};

#endif
