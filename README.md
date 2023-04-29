# Timer

Minimal C++ library to schedule function calls on an arduino framework platform.
It supports timeouts, intervals, and timeout intervals.

## Installation

Add following line to "lib_deps" section in `platformio.ini`:

```
uniuno/timer@^1.0.2
```

so it should look like:

```
; other config sections...

lib_deps =
; other deps...
	uniuno/timer@^1.0.2
```

## Usage

### Creating an instance and attaching to loop

1. Add following header to your main.cpp file:

```cpp
#include <timer.h>
```

2. Create a new instance of Timer class:

```cpp
uniuno::Timer timer;
```

3. Schedule a function in the "setup" and attach the timer to the loop:

```cpp
// ...

void setup() {
  // ...

  // passed lambda will call in 5 seconds
  timer.set_timeout([]() { Serial.println("5 seconds has passed"); }, 5000);

  // attach timer to the loop
  timer.attach_to_loop();
}
// ...
```

4. Call tick method in the "loop":

```cpp
// ...
void loop() {
  // ...
  timer.tick();
  // ...
}
// ...
```

### API

#### set_timeout

Calls function after specified amount of time:

```cpp
  auto fn = []() { Serial.println("5 seconds has passed"); };
  unsigned long timeout = 5000;

  timer.set_timeout(fn, timeout);
```

#### set_interval

Calls function at specified interval:

```cpp
  auto fn = []() { Serial.println("1 second has passed"); };
  unsigned long interval = 1000;

  timer.set_interval(fn, interval);
```

#### set_interval_until

Calls function at specified interval for specified amount of time:

```cpp
  auto intervalFn = []() { Serial.println("1 second has passed"); };
  unsigned long interval = 1000;
  auto onTimeout = []() { Serial.println("5 seconds has passed")};
  unsigned long timeout = 5000;

  timer.set_interval_until(intervalFn, interval, onTimeout, timeout);
```

#### set_immediate

Calls function immediately:

```cpp
  auto fn = []() { Serial.println("immediate call"); };

  timer.set_immediate(fn);
```

#### set_on_loop

Calls function at every loop call:

```cpp
  auto fn = []() { Serial.println("loop call (tick)"); };

  timer.set_on_loop(fn);
```

#### set_on_loop_until

Calls function at every loop call for the specified amount of time:

```cpp
  auto fn = []() { Serial.println("loop call (tick)"); };
  auto onTimeout = []() { Serial.println("5 seconds has passed")};
  unsigned long timeout = 5000;

  timer.set_on_loop_until(fn, onTimeout, timeout);
```

#### clear_timeout

Removes scheduled timeout if scheduled function has not been called yet:

```cpp
  // function scheduled to be called in 5 seconds (it won't be called as we cancel it)
  auto fn = []() { Serial.println("5 seconds has passed"); };
  unsigned long timeout = 5000;
  auto timeoutId = timer.set_timeout(fn, timeout);

  // function to clear the timeout before it is executed (in 4 seconds)
  auto cancelFn = [timeoutId]() { timer.clear_timeout(timeoutId); };
  timer.set_timeout(cancelFn, 4000);

```

#### clear_interval

Removes scheduled interval - stops calling interval function:

```cpp
  // function scheduled to be called every 1 seconds (we'll stop calling it after 4 seconds)
  auto fn = []() { Serial.println("1 seconds has passed"); };
  unsigned long interval = 1000;

  auto intervalId = timer.set_interval(fn, timeout);

  // function to clear the interval after 4 seconds
  auto cancelFn = [intervalId]() { timer.clear_interval(intervalId); };
  timer.set_timeout(cancelFn, 4000);

```

#### attach_to_loop

Attaches timer instance to the loop. When timer is attached the timeout and interval functions will be executed.

#### detach_from_loop

Detaches timer instance from the loop. When timer is detached no scheduled functions will be executed.

#### tick

Method responsible for calling scheduled functions on time.
It has to be executed at every single "loop" function call.

#### constructor

When creating a new instance of Timer you can specify "clock" function that returns current time in a unit of your choice (default is milliseconds):

For example for microseconds:

```cpp
// ...

unsigned long micros() {
  return std::chrono::system_clock::now().time_since_epoch() /
         std::chrono::microseconds(1);
};

// create microseconds timer
uniuno::Timer timer(micros);

// ...
```

## Licence

MIT.
