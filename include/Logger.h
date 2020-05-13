#ifndef LOGGER_H
#define LOGGER_H

class ILogger {
 public:
  virtual void log(const char *pMessage) = 0;
  virtual void log(const char *pMessage, const unsigned long value) = 0;
  virtual void log(const char *pMessage, const float value) = 0;
  virtual void log(const char *pMessage, const bool value) = 0;
  virtual ~ILogger() = 0;
};
inline ILogger::~ILogger() {}

class SerialLogger : public ILogger {
 public:
  SerialLogger(const char *pLoggerName);

  void log(const char *pMessage) override;
  void log(const char *pMessage, const unsigned long value) override;
  void log(const char *pMessage, const float value) override;
  void log(const char *pMessage, const bool value) override;

 private:
  const char *pLoggerName;
};

class DoNothingLogger : public ILogger {
 public:
  DoNothingLogger();

  void log(const char *pMessage) override;
  void log(const char *pMessage, const unsigned long value) override;
  void log(const char *pMessage, const float value) override;
  void log(const char *pMessage, const bool value) override;
};

#endif