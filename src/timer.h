#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#ifndef ARDUINO
unsigned long millis() {
  return std::chrono::system_clock::now().time_since_epoch() /
         std::chrono::milliseconds(1);
};
#endif

namespace uniuno {

class Timer {
public:
  Timer(std::function<unsigned long(void)> get_now_in_time_unit = millis) {
    this->now = get_now_in_time_unit;
  }

  unsigned int set_timeout(std::function<void(void)> callback,
                           unsigned long timeout_ms) {
    this->timeouts.push_back(
        Timeout{this->index, this->now() + timeout_ms, callback});

    std::sort(this->timeouts.begin(), this->timeouts.end(),
              [](Timeout t1, Timeout t2) {
                return t1.next_call_time < t2.next_call_time;
              });

    return this->index++;
  }

  unsigned int set_immediate(std::function<void(void)> callback) {
    return this->set_timeout(callback, 0);
  }

  unsigned int set_interval(std::function<void(void)> callback,
                            unsigned long interval_time) {
    this->intervals.push_back(Interval{this->index, this->now() + interval_time,
                                       interval_time, [callback]() {
                                         callback();
                                         return false;
                                       }});

    return this->index++;
  }

  unsigned int set_on_loop(std::function<void(void)> callback) {
    return this->set_interval(callback, 0);
  }

  void clear_timeout(unsigned int timeout_id) {
    this->timeout_ids_to_remove.push_back(timeout_id);
  }

  void clear_interval(unsigned int interval_id) {
    this->interval_ids_to_remove.push_back(interval_id);
  }

  unsigned int set_interval_until(std::function<bool(void)> callback,
                                  unsigned long interval_time,
                                  std::function<void(void)> on_timeout,
                                  unsigned long timeout_time) {
    auto timeout_call_ms = this->now() + timeout_time;

    return this->set_interval_until(
        [this, timeout_call_ms, callback, on_timeout]() {
          bool should_stop = callback();
          if (!should_stop && timeout_call_ms <= this->now()) {
            on_timeout();
            return true;
          }

          return should_stop;
        },
        interval_time);
  }

  unsigned int set_interval_until(std::function<bool(void)> callback,
                                  unsigned long interval_time) {
    this->intervals.push_back(Interval{this->index, this->now() + interval_time,
                                       interval_time, callback});

    return this->index++;
  }

  unsigned int set_on_loop_until(std::function<bool(void)> callback,
                                 std::function<void(void)> on_timeout,
                                 unsigned long timeout_time) {
    return this->set_interval_until(callback, 0, on_timeout, timeout_time);
  }

  unsigned int set_on_loop_until(std::function<bool(void)> callback) {
    return this->set_interval_until(callback, 0);
  }

  void attach_to_loop() { this->attached = true; }
  void detach_from_loop() { this->attached = false; }

  void tick() {
    if (!this->attached) {
      return;
    }

    if (!this->interval_ids_to_remove.empty()) {
      for (uint16_t i = 0; i < this->intervals.size(); i++) {
        for (uint16_t j = 0; j < this->interval_ids_to_remove.size(); j++) {
          if (this->intervals[i].id == this->interval_ids_to_remove[j]) {
            this->intervals.erase(this->intervals.begin() + i);
            this->interval_ids_to_remove.erase(
                this->interval_ids_to_remove.begin() + j);
            break;
          }
        }
      }
      this->interval_ids_to_remove.clear();
      return;
    }

    if (!this->timeout_ids_to_remove.empty()) {
      for (uint16_t i = 0; i < this->timeouts.size(); i++) {
        for (uint16_t j = 0; j < this->timeout_ids_to_remove.size(); j++) {
          if (this->timeouts[i].id == this->timeout_ids_to_remove[j]) {
            this->timeouts.erase(this->timeouts.begin() + i);
            this->timeout_ids_to_remove.erase(
                this->timeout_ids_to_remove.begin() + j);
            break;
          }
        }
      }
      this->timeout_ids_to_remove.clear();
      return;
    }

    for (std::size_t i = 0; i < this->timeouts.size(); i++) {
      if (this->now() < this->timeouts[i].next_call_time) {
        break;
      }

      this->timeouts[i].callback();
      this->timeouts.erase(this->timeouts.begin() + i);
    }

    for (std::size_t i = 0; i < this->intervals.size(); i++) {
      if (this->now() < this->intervals[i].next_call_time) {
        continue;
      }

      if (this->intervals[i].callback()) {
        this->intervals.erase(this->intervals.begin() + i);
      } else {
        this->intervals[i].next_call_time =
            this->now() + this->intervals[i].interval_time;
      }
    }
  }

private:
  std::function<unsigned long(void)> now;

  unsigned int index = 0;

  struct Timeout {
    unsigned int id;
    unsigned long next_call_time;
    std::function<void(void)> callback;
  };

  std::vector<Timeout> timeouts;

  struct Interval {
    unsigned int id;
    unsigned long next_call_time;
    unsigned long interval_time;
    std::function<bool(void)> callback;
  };

  std::vector<Interval> intervals;
  std::vector<unsigned int> interval_ids_to_remove;
  std::vector<unsigned int> timeout_ids_to_remove;

  bool attached = false;
};

} // namespace uniuno