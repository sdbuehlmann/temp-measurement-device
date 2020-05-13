#include "Logger.h"
#include <Arduino.h>

// loggers

SerialLogger::SerialLogger(const char *pLoggerName){
  this->pLoggerName = pLoggerName;
}

void SerialLogger::log(const char *pMessage) {
  Serial.print(pLoggerName);
  Serial.print(": ");
  Serial.println(pMessage);
}

void SerialLogger::log(const char *pMessage, const unsigned long value) {
  Serial.print(pLoggerName);
  Serial.print(": ");
  Serial.print(pMessage);
  Serial.println(value);
}

void SerialLogger::log(const char *pMessage, const float value) {
  Serial.print(pLoggerName);
  Serial.print(": ");
  Serial.print(pMessage);
  Serial.println(value);
}

void SerialLogger::log(const char *pMessage, const bool value) {
  Serial.print(pLoggerName);
  Serial.print(": ");
  Serial.print(pMessage);
  Serial.println(value);
}

DoNothingLogger::DoNothingLogger() {}

void DoNothingLogger::log(const char *pMessage) {}
void DoNothingLogger::log(const char *pMessage, const unsigned long value) {}
void DoNothingLogger::log(const char *pMessage, const float value) {}
void DoNothingLogger::log(const char *pMessage, const bool value) {}