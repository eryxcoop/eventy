# Eventy

Eventy is a framework that lets you seamleslly run parallel tasks and provides a PubSub communication system.
Usually eventy systems have the following structure:

![eventy](https://user-images.githubusercontent.com/17937384/185474592-1648d170-4e9b-4564-9ed6-39c4247007a2.png)

The top tasks are the main producers of events, often sensors, running at regular intervals of time and pushing data to certain topics.
The events handlers are subscribed to the topics and consume from those events, processing the data and optionally producing events too. These run only on event delivery. They need to be fast since events are delivered to them sequentially and a slow event handler would block the rest.
The bottom tasks are also consumers, either too slow to be event handlers or that need to run at regular intervals independently of events.
Examples of consumers are:
* Loggeres that write to a SD card every event and produce an event indicating a correct write.
* A module that keeps statistics about the data received and exposes methods to query them.
* A module that sends those statistics at regular intervals through LoRa or to a MQTT broker.
* A display, a led or a buzzer that indicates the status of the system to the user based on the computed statistics or the number of events received.
One of the main advantages of event driven designs is the decoupling of the componentes that produce events from the ones that consume them.

# Blink

A dummy example. This does not showcase the important features of the library.

```c++
#include "Arduino.h"
#include "Eventy.h"

using namespace eventy;

Eventy eventy_manager;
bool led_status = LOW;

EventCollection toggleLed() {
    led_status = !led_status;
    digitalWrite(LED_BUILTIN, led_status);

    return {};
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    eventy_manager.registerTask(toggleLed, 1000);
    vTaskDelete(NULL);
}

void loop() {
}
```

# The eventy namespace
Eventy exposes everything under the namespace `eventy`. If you are unfamiliar with namespaces, this only means that you need to prepend `eventy::` to the names of symbols imported from this library. You can avoid this by adding the following line in your files.
```c++
using namespace eventy;
```

# Instantiation of the eventy manager
The eventy manager is simply created as follows
```c++
Eventy eventy_manager;
```

# Defining tasks
Tasks and event handlers can either be defined with functions or classes.

## Functions
Define tasks using functions as follows. The function needs to take no parameters and return a `EventCollection`. Events are created using `EventFactory::create` that takes as first parameter the topic and an optional second parameter a value that will be attached to the event. Events are added to the collection using the `push` method. Let's see this in an example.
```c++
EventCollection myFunction() {
    EventCollection events;

    float a_float = 1.5f;
    events.push(EventFactory::create("my-topic/A", a_float));

    int an_integer = 7;
    events.push(EventFactory::create("my-topic/B", an_integer));

    events.push(EventFactory::create("my-topic/no-value"));

    return events;
}
```
The first event created will be of type `Event<float>`, the second event will be of type `Event<int>`. The last event that has no value is of type `EventWithoutValue`. This will be important later for handling events. In general if a variable `value` of type `T` is passed to the second parameter of `EventFactory::create`, an event of type `Event<T>` is created.

## Register a function task
Once you have defined your function, you can register it for eventy to start running it. You need only to specify the periodicity in milliseconds.
```c++
eventy_manager.registerTask(myFunction, 1000);
```
This will already run `myFunction` every one second in its own thread.


## Event handlers
TODO

## Classes
TODO

## Restarting malfunctioning tasks
The user of eventy can easily define what it means for every task to be healthy and ready to run. Healthy tasks will then run at the specified intervals. Otherwise eventy will restart them using an exponential backoff retry algorithm.

# Out of the box thread safe event handlers
All of the tasks run in different threads of control. In a special thread run all the event handler calls when events are delivered to them. Special attention is needed for resources that are shared between tasks and event handlers. An example would be an event handler that listens and stores the last value produced by a sensor and exposes a method to get it. If a task calls such a method while the event handler is updating it, incorrect values could be returned.
For this reason eventy provides general purpose thread safe event handlers for storing data and statistics. We strongly encourage to use them. Feel free to open an issue if they don't meet your needs.
TODO
