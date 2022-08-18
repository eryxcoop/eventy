#ifndef EVENTY_METRIC_H
#define EVENTY_METRIC_H

#include "Stack.tpp"
#include "EventHandler.h"
#include "Event.h"
#include "time.h"
#include "string.h"

namespace eventy {

template<class T>
class TimeSeries {
public:
    TimeSeries(unsigned int size) {
        _size = size;
        _buffer_ptr = new Record[_size];
        _mutex = xSemaphoreCreateMutex();
    }
    ~TimeSeries() {
        delete[] _buffer_ptr;
        vSemaphoreDelete(_mutex);
    }

    class Record {
    public:
        Record() : object(T()), timestamp(-1) {}
        Record(T object, time_t timestamp) : object(object), timestamp(timestamp) {}
        T object;
        time_t timestamp;
    };

    void add(T object) {
        if (block() != pdFALSE) {
            _buffer_ptr[_position] = Record(object, time(NULL));
            _position = (_position + 1) % _size;
            unblock();
        }
    }
    Record *getBufferArray() { return _buffer_ptr; }
    BaseType_t block() { return xSemaphoreTake(_mutex, 100); }
    void unblock() { xSemaphoreGive(_mutex); }
    unsigned int size() { return _size; }
private:
    SemaphoreHandle_t _mutex;
    unsigned int _size;
    unsigned int _position = 0;
    Record *_buffer_ptr;
};

class Metric : public EventHandler {
};

class RollingMean : public Metric {
public:
    RollingMean(unsigned int size);
    ~RollingMean();
    EventCollection handle(EventPtr event_ptr) override;
    double value(time_t from_time) const;
private:
    TimeSeries<double> *_buffer;
};

class PeriodCounter : public Metric {
public:
    PeriodCounter(unsigned int size);
    ~PeriodCounter();
    EventCollection handle(EventPtr event_ptr) override;
    int value(time_t from_time) const;
private:
    TimeSeries<bool> *_buffer;
};

template<class T>
class LatestValue : public Metric {
public:
    LatestValue() { _mutex = xSemaphoreCreateMutex(); }

    EventCollection handle(EventPtr event_ptr) override {
        if (xSemaphoreTake(_mutex, 1000) != pdFALSE) {
            _latest_value = ((Event<T> *) event_ptr)->value();
            xSemaphoreGive(_mutex);
        }
        return {}; // TODO: maybe return info message?
    }

    T value() const {
        if (xSemaphoreTake(_mutex, 1000) != pdFALSE) {
            T latest_value = _latest_value;
            xSemaphoreGive(_mutex);
            return latest_value;
        }
        return {};
    }

private:
    xSemaphoreHandle _mutex;
    T _latest_value;
};

}

#endif  // EVENTY_METRIC_H
