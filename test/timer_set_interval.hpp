#include <array>
#include <chrono>
#include <functional>
#include <helpers.hpp>
#include <timer.h>
#include <unity.h>

void test_set_interval_callback_is_executed_in_specified_interval(void) {
  uniuno::Timer timer;
  timer.attach_to_loop();
  CallbackStub callback_stub;

  timer.set_interval(callback_stub.get_callback(), 10);
  tick_for(51, timer);

  TEST_ASSERT_EQUAL_INT32(5, callback_stub.calls_number);
}

void test_set_interval_callback_is_not_executed_if_timer_is_detached(void) {
  uniuno::Timer timer;
  CallbackStub callback_stub;

  timer.set_interval(callback_stub.get_callback(), 50);
  tick_for(11, timer);

  TEST_ASSERT_EQUAL_INT32(0, callback_stub.calls_number);
}

void test_set_interval_callback_is_not_executed_if_interval_has_been_cleared_before_first_execution(
    void) {
  uniuno::Timer timer;
  timer.attach_to_loop();
  CallbackStub callback_stub;

  auto id = timer.set_interval(callback_stub.get_callback(), 20);
  tick_for(19, timer);
  timer.clear_interval(id);
  tick_for(12, timer);

  TEST_ASSERT_EQUAL_INT32(0, callback_stub.calls_number);
}

void test_set_interval_callback_and_related_callbacks_are_executed_in_specified_interval_if_callback_sets_additional_intervals(
    void) {
  uniuno::Timer timer;
  timer.attach_to_loop();
  CallbackStub callback_stub2;
  CallbackStub callback_stub3;
  CallbackStub callback_stub4;
  CallbackStub callback_stub5;

  CallbackStub callback_stub1([&]() {
    timer.set_interval(callback_stub2.get_callback(), 10);
    timer.set_interval(callback_stub3.get_callback(), 20);
    timer.set_interval(callback_stub4.get_callback(), 30);
    timer.set_interval(callback_stub5.get_callback(), 40);
  });

  timer.set_interval(callback_stub1.get_callback(), 10);
  tick_for(101, timer);

  TEST_ASSERT_GREATER_OR_EQUAL(8, callback_stub1.calls_number);
  TEST_ASSERT_GREATER_OR_EQUAL(45, callback_stub2.calls_number);
  TEST_ASSERT_GREATER_OR_EQUAL(20, callback_stub3.calls_number);
  TEST_ASSERT_GREATER_OR_EQUAL(12, callback_stub4.calls_number);
  TEST_ASSERT_GREATER_OR_EQUAL(2, callback_stub5.calls_number);
}

void test_set_interval_executes_interconnected_intervals_with_the_use_of_set_interval_until(
    void) {
  uniuno::Timer timer;
  timer.attach_to_loop();
  std::array<CallbackStub, 16> callback_stubs;
  for (uint8_t i = 0; i < callback_stubs.size(); i++) {
    callback_stubs[i] = CallbackStub();
  }

  uint8_t counter = 0;
  timer.set_interval_until(
      [&]() {
        if (counter++ == callback_stubs.size()) {
          for (uint8_t i = 0; i < callback_stubs.size(); i++) {
            timer.set_interval(callback_stubs[i].get_callback(), 10 + i);
          }
          return true;
        }
        return false;
      },
      11);
  tick_for(100 * callback_stubs.size(), timer);

  TEST_ASSERT_EQUAL(callback_stubs.size() + 1, counter);
  for (uint8_t i = 0; i < callback_stubs.size(); i++) {
    printf("testing callback %u\n", i);
    TEST_ASSERT_GREATER_OR_EQUAL(1, callback_stubs[i].calls_number);
  }
}