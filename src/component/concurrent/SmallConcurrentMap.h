//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_SMALLCONCURRENTMAP_H
#define QUIK_CONNECTOR_SMALLCONCURRENTMAP_H

#include <string>
#include <mutex>
#include <list>
#include "../../../src/dto/option/Option.h"

using namespace std;

template<typename K, typename V> class MapEntry {
public:
    K key;
    V value;
};

/**
 * Simple implementation of the thread-safe map based on the std::list.
 *
 * <p>We create this implementation to avoid use exists difficult libraries
 * such as libcds and etc.
 *
 * <p>This concurrent map may be used only for a small amount of the keys.
 * On the large amount of the keys we can get bad performance due a lot of
 * locks for each method.
 *
 * <p>Iterator method create a copy of the list so it can be safely iterate
 * and because there a few keys we can get good performance.
 *
 * @tparam K key type
 * @tparam V value type
 */
template<typename K, typename V> class SmallConcurrentMap {
public:
    Option<V> get(const K& key) {
        lock_guard<mutex> lockGuard(lock);

        for (auto& item : entryList) {
            if (item.key == key) {
                return Option<V>(item.value);
            }
        }
        return Option<V>();
    }

    bool isEmpty() {
        return entryList.empty();
    }

    int getSize() {
        return entryList.size();
    }

    list<MapEntry<K, V>> iterator() {
        list<MapEntry<K, V>> tmpList;

        lock_guard<mutex> lockGuard(lock);

        for (auto& item : entryList) {
            tmpList.push_back(item);
        }
        return tmpList;
    }

    bool containsKey(const K& key) {
        lock_guard<mutex> lockGuard(lock);

        for (auto& item : entryList) {
            if (item.key == key) {
                return true;
            }
        }
        return false;
    }

    void put(const K& key, V value) {
        lock_guard<mutex> lockGuard(lock);

        for (auto& item : entryList) {
            if (item.key == key) {
                item.value = value;

                return;
            }
        }
        MapEntry<K, V> mapEntry;
        mapEntry.key = key;
        mapEntry.value = value;

        entryList.push_back(mapEntry);
    }

    void remove(const K& key) {
        lock_guard<mutex> lockGuard(lock);

        for (auto item = entryList.begin(); item != entryList.end(); ++item) {
            if (item->key == key) {
                entryList.erase(item);

                break;
            }
        }
    }
private:
    mutex lock;
    list<MapEntry<K, V>> entryList;
};

#endif //QUIK_CONNECTOR_SMALLCONCURRENTMAP_H
