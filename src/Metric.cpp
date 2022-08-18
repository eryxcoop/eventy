#include "eventy/Metric.h"

namespace eventy {

// RollingMean

RollingMean::RollingMean(unsigned int size) {
    _buffer = new TimeSeries<double>(size);
}

RollingMean::~RollingMean() {
    delete _buffer;
}

EventCollection RollingMean::handle(EventPtr event_ptr) {
    double new_value;
    switch (event_ptr->type()) {
        case EVENT_TYPE_BOOL: {
            new_value = (double) ((Event<bool> *) event_ptr)->value();
            break;
        }
        case EVENT_TYPE_INT: {
            new_value = (double) ((Event<int> *) event_ptr)->value();
            break;
        }
        case EVENT_TYPE_UINT: {
            new_value = (double) ((Event<unsigned int> *) event_ptr)->value();
            break;
        }
        case EVENT_TYPE_LONG: {
            new_value = (double) ((Event<long> *) event_ptr)->value();
            break;
        }
        case EVENT_TYPE_ULONG: {
            new_value = (double) ((Event<unsigned long> *) event_ptr)->value();
            break;
        }
        case EVENT_TYPE_FLOAT: {
            new_value = (double) ((Event<float> *) event_ptr)->value();
            break;
        }
        case EVENT_TYPE_DOUBLE: {
            new_value = ((Event<double> *) event_ptr)->value();
            break;
        }
        default: {
            return {};
        }
    }
    _buffer->add(new_value);
    return {};
}

double RollingMean::value(time_t from_time) const {
    double result = 0;
    unsigned int num_samples = 0;
    if (_buffer->block() != pdFALSE) {
        TimeSeries<double>::Record *buffer_array = _buffer->getBufferArray();
        for (int i = 0; i < _buffer->size(); i++) {
            if ((buffer_array[i].timestamp > -1) && (buffer_array[i].timestamp > from_time)) {
                result += buffer_array[i].object;
                num_samples++;
            }
        }
        _buffer->unblock();
    }
    return num_samples > 0 ? result / num_samples : 0;
}

// PeriodCounter

PeriodCounter::PeriodCounter(unsigned int size) {
    _buffer = new TimeSeries<bool>(size);
}

PeriodCounter::~PeriodCounter() {
    delete _buffer;
}

EventCollection PeriodCounter::handle(EventPtr event_ptr) {
    _buffer->add(true);
    return {};
}

int PeriodCounter::value(time_t from_time) const {
    int result = 0;
    if (_buffer->block() != pdFALSE) {
        TimeSeries<bool>::Record *buffer_array = _buffer->getBufferArray();
        for (int i = 0; i < _buffer->size(); i++) {
            if ((buffer_array[i].timestamp > -1) && (buffer_array[i].timestamp > from_time)) {
                result++;
            }
        }
        _buffer->unblock();
    }
    return result;
}

}
