/*
 * timer_timeout
 *
 * Switches on the built-in LED diode after specified amount of time.
 */

#include <timer.h>

uniuno::Timer timer;

void toggleLed() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggles built-in LED
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT

  timer.set_timeout(toggleLed, 5000); // schedules "toggleLed" function to be called after 5000ms of wait (non-blocking)
  timer.attach_to_loop(); // attaches timer to the loop ticker

  // you can run your code here
}

void loop() {  
  timer.tick(); // tick the timer

  // you can run your code here
}