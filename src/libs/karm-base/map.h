#pragma once

#include "cons.h"
#include "std.h"
#include "vec.h"

namespace Karm {

template <typename K, typename V>
struct Map {
    Vec<Cons<K, V>> _els{};

    Map() = default;

    Map(std::initializer_list<Cons<K, V>> &&list)
        : _els(std::move(list)) {}

    void put(K const &key, V value) {
        for (auto &i : ::mutIter(_els)) {
            if (i.car == key) {
                i.cdr = value;
                return;
            }
        }

        _els.pushBack(Cons<K, V>{key, value});
    }

    Opt<V> get(K const &key) const {
        for (auto &i : _els) {
            if (i.car == key) {
                return i.cdr;
            }
        }

        return NONE;
    }

    bool del(K const &key) {
        for (usize i = 0; i < _els.len(); i++) {
            if (_els[i].car == key) {
                _els.removeAt(i);
                return true;
            }
        }

        return false;
    }

    auto iter() {
        return mutIter(_els);
    }

    auto iter() const {
        return ::iter(_els);
    }

    usize len() const {
        return _els.len();
    }

    void clear() {
        _els.clear();
    }
};

} // namespace Karm
