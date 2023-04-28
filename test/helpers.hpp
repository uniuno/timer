#pragma once

#include <chrono>
#include <functional>
#include <timer.h>

class CallbackStub {
public:
  CallbackStub(std::function<void(void)> on_called = []() {})
      : on_called(on_called) {}

  std::function<void(void)> get_callback() {
    return [this]() {
      this->on_called();
      this->calls_number++;
    };
  }

  void reset() { this->calls_number = 0; }

  uint32_t calls_number = 0;

private:
  std::function<void(void)> on_called = []() {};
};

void tick_for(unsigned long time, uniuno::Timer &timer) {
  unsigned long current_time = millis();
  while (millis() < current_time + time) {
    timer.tick();
  }
}