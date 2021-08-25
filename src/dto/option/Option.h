//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_OPTION_H
#define QUIK_CONNECTOR_OPTION_H

template <typename T> class Option {
public:
    Option() {
        empty = true;
    }

    Option(T newData) {
        empty = false;
        data = newData;
    }

    T get() {
        return data;
    }

    bool isPresent() {
        return !empty;
    }

    bool isEmpty() {
        return empty;
    }

private:
    T data;

    bool empty;
};

#endif //QUIK_CONNECTOR_OPTION_H
