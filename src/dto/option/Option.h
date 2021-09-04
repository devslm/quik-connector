//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_OPTION_H
#define QUIK_CONNECTOR_OPTION_H

template <typename T> class Option {
public:
    Option() {
        this->empty = true;
    }

    Option(T newData) {
        this->empty = false;
        this->data = newData;
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
