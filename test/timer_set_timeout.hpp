#include <chrono>
#include <functional>
#include <helpers.hpp>
#include <timer.h>
#include <unity.h>

void test_set_timeout_callback_is_executed_after_specified_time(void) {
  uniuno::Timer timer;
  timer.attach_to_loop();
  CallbackStub callback_stub;

  timer.set_timeout(callback_stub.get_callback(), 50);
  TEST_ASSERT_EQUAL_INT32(0, callback_stub.calls_number);
  tick_for(51, timer);

  TEST_ASSERT_EQUAL_INT32(1, callback_stub.calls_number);
}

void test_set_timeout_callback_is_not_executed_after_specified_time_if_timer_is_detached(
    void) {
  uniuno::Timer timer;
  CallbackStub callback_stub;

  timer.set_timeout(callback_stub.get_callback(), 50);
  tick_for(60, timer);

  TEST_ASSERT_EQUAL_INT32(0, callback_stub.calls_number);
}

void test_set_timeout_callback_and_related_callbacks_are_executed_after_specified_time_if_it_sets_additional_timeouts(
    void) {
  uniuno::Timer timer;
  timer.attach_to_loop();
  CallbackStub callback_stub2;
  CallbackStub callback_stub3;
  CallbackStub callback_stub4;
  CallbackStub callback_stub5;

  CallbackStub callback_stub1([&]() {
    timer.set_timeout(callback_stub2.get_callback(), 10);
    timer.set_timeout(callback_stub3.get_callback(), 20);
    timer.set_timeout(callback_stub4.get_callback(), 30);
    timer.set_timeout(callback_stub5.get_callback(), 40);
  });

  timer.set_timeout(callback_stub1.get_callback(), 10);
  tick_for(51, timer);

  TEST_ASSERT_EQUAL_INT32(1, callback_stub1.calls_number);
  TEST_ASSERT_EQUAL_INT32(1, callback_stub2.calls_number);
  TEST_ASSERT_EQUAL_INT32(1, callback_stub3.calls_number);
  TEST_ASSERT_EQUAL_INT32(1, callback_stub4.calls_number);
  TEST_ASSERT_EQUAL_INT32(1, callback_stub5.calls_number);
}