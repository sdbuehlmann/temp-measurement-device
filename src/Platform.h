#ifndef PLATFORM_H
#define PLATFORM_H

// includes
#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>

#include "EdgeDetector.h"

// defines
#define DEBUG_PRINT(x) Serial.println(x);

#define DHTPIN 15
#define DHTTYPE DHT22

#define LED_OFF 0
#define LED_ON 1

#define LED_BUILTIN 5
#define BUTTON_1 17
#define BUTTON_2 16
#define BUTTON_3 4

#define NR_DEBOUNCE_CYCLES 40

#define WIFI_DISCONNECTED 0
#define WIFI_CONNECTED 1

#define BUTTON_ACTIVE 2
#define BUTTON_SWITCHED_ACTIVE 1
#define BUTTON_SWITCHED_DEACTIVE -1
#define BUTTON_DEACTIVE -2

struct MeasurementValues {
  float temperature;
  float humidity;
};

class Platform {
 public:
  static Platform& instance() {
    static Platform _instance;
    return _instance;
  }
  int readButton(int buttonId);
  int readButton1();
  int readButton2();
  int readButton3();

  /**
   * LED_ON or LED_OFF
   */
  void switchLed(int state);

  void connectWifi() {
    DEBUG_PRINT("Connecting wifi...");
    WiFi.begin("Gondor", "Ib1PWume!");
  }
  void disconnectWifi() {
    DEBUG_PRINT("Disconnecting wifi...");
    WiFi.disconnect();
  }
  int getWifiStatus() {
    if (WiFi.status() == WL_CONNECTED) {
      return WIFI_CONNECTED;
    }
    return WIFI_DISCONNECTED;
  }
  char* getSsid() { return "Gondor"; }
  char* getIpAdress() {
    static char printedIp[4 * 3 + 3];

    if (getWifiStatus() == WIFI_CONNECTED) {
      IPAddress ip = WiFi.localIP();
      sprintf(printedIp, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

      return printedIp;
    }
    return "-";
  }

  void proceed();

  MeasurementValues measure() {
    return {dht.readTemperature(), dht.readHumidity()};
  }

 private:
  Platform();
  class Platform& operator=(
      const class
      Platform&);  // https://de.wikibooks.org/wiki/C%2B%2B-Programmierung:_Entwurfsmuster:_Singleton

  // variables
  EdgeDetector button1 = EdgeDetector(BUTTON_1, NR_DEBOUNCE_CYCLES);
  EdgeDetector button2 = EdgeDetector(BUTTON_2, NR_DEBOUNCE_CYCLES);
  EdgeDetector button3 = EdgeDetector(BUTTON_3, NR_DEBOUNCE_CYCLES);

  DHT dht = DHT(DHTPIN, DHTTYPE);

  // helpers
  int readButton(EdgeDetector& edgeDetector);
};

Platform::Platform() {
  pinMode(LED_BUILTIN, OUTPUT);
  dht.begin();
}

/**
 * Returns:
 *  BUTTON_ACTIVE: Button was already active before last proceed() call
 *  BUTTON_SWITCHED_ACTIVE: Button was switched active in last proceed() call
 *  BUTTON_SWITCHED_DEACTIVE: Button was switched deactive (released) in last
 * proceed() call BUTTON_DEACTIVE: Button was already deactive (released),
 * before last proceed() call
 */
int Platform::readButton(EdgeDetector& edgeDetector) {
  switch (edgeDetector.getLastEdge()) {
    case NO_CHANGE:
      if (edgeDetector.getState() == HIGH) {
        return BUTTON_ACTIVE;
      }
      return BUTTON_DEACTIVE;
    case RISING_EDGE:
      return BUTTON_SWITCHED_ACTIVE;
    case FALLING_EDGE:
      return BUTTON_SWITCHED_DEACTIVE;
  }

  return 0;
}

int Platform::readButton(int buttonId) {
  switch (buttonId) {
    case 1:
      return readButton(button1);
    case 2:
      return readButton(button2);
    case 3:
      return readButton(button3);

    default:
      return BUTTON_DEACTIVE;
  }
}

int Platform::readButton1() { return readButton(button1); }

int Platform::readButton2() { return readButton(button2); }

int Platform::readButton3() { return readButton(button3); }

void Platform::switchLed(int state) {
  switch (state) {
    case LED_OFF:
      digitalWrite(LED_BUILTIN, HIGH);
      break;
    case LED_ON:
      digitalWrite(LED_BUILTIN, LOW);
      break;
    default:
      break;
  }
}

void Platform::proceed() {
  button1.proceed();
  button2.proceed();
  button3.proceed();
}

#endif
