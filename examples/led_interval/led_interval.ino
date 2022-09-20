/*
 * timer_interval
 *
 * Blinks the built-in LED diode every second.
 */

#include <timer.h>

uniuno::Timer timer;

void toggleLed() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggles built-in LED
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT

  timer.set_interval(toggleLed, 1000); // schedules "toggleLed" function to be called at 1000ms interval
  timer.attach_to_loop(); // attaches timer to the loop ticker (starts the interval calls)

  // you can run your code here
}

void loop() {
  timer.tick(); // tick the timer

  // you can run your code here
}