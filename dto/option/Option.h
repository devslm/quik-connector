//
// Created by Sergey on 25.06.2021.
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
