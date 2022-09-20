/*
 * timer_timeout_interval
 *
 * Blinks the built-in LED diode at specified interval for specified amount of time.
 */

#include <timer.h>

uniuno::Timer timer;

void toggleLed() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggles built-in LED
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT

  timer.set_interval_until(toggleLed, 1000, []() {
    // last function to be called when timeout time is reached
  }, 5000); // schedules "toggleLed" function to be called every 1000ms for 5000ms (non-blocking)
  timer.attach_to_loop(); // attaches timer to the loop ticker

  // you can run your code here
}

void loop() {  
  timer.tick(); // tick the timer

  // you can run your code here
}