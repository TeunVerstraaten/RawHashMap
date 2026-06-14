
#ifndef __OPEN_HASH_MAP_H__
#define __OPEN_HASH_MAP_H__

#include <bit>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <new>
#include <vector>

template <typename K, typename V>
class OpenHashMap {
    size_t               _capacity;
    size_t               _occupied;
    double               _resize_threshold;
    std::vector<K>       _keys;
    std::vector<V>       _values;
    std::vector<uint8_t> _free;

    void resize(size_t new_size) {
        if (new_size <= _capacity) {
            return;
        }
        auto rounded_new_size = std::__bit_ceil(new_size);
        auto new_hm           = OpenHashMap<K, V>(rounded_new_size);
        for (size_t i = 0; i != _free.size(); ++i) {
            if (!_free[i]) {
                new_hm.insert_in_next_free_slot(std::move(_keys[i]), std::move(_values[i]));
            }
        }

        std::swap(_capacity, new_hm._capacity);
        std::swap(_occupied, new_hm._occupied);
        std::swap(_keys, new_hm._keys);
        std::swap(_values, new_hm._values);
        std::swap(_free, new_hm._free);
    }

    void insert_in_next_free_slot(K&& key, V&& value) {
        auto i = get_index(key);
        while (!_free[i]) {
            ++i;
            i &= _capacity - 1;
        }

        _keys[i]   = std::move(key);
        _values[i] = std::move(value);
        _free[i]   = 0;
        ++_occupied;
    }

    size_t get_index(const K& key) const {
        static const auto hasher = std::hash<K>{};
        auto              h      = hasher(key);
        return h & (_capacity - 1);
    }

    void resize_if_needed() {
        if (_capacity * _resize_threshold <= _occupied) {
            resize(_capacity * 2);
        }
    }

  public:
    OpenHashMap(size_t size_hint = 32, double resize_threshold = 0.7)
        : _capacity(std::__bit_ceil(size_hint)),
          _occupied(0),
          _resize_threshold(resize_threshold),
          _keys(std::vector<K>(_capacity, K{})),
          _values(std::vector<V>(_capacity, V{})),
          _free(_capacity, 1) {
        assert(resize_threshold <= 1.0);
    }

    void show() const {
        for (size_t i = 0; i != _capacity; ++i) {
            if (!_free[i]) {
                std::cout << "i = " << i << " => " << _keys[i] << ": " << _values[i] << std::endl;
            }
        }
    }

    size_t capacity() const {
        return _capacity;
    }

    size_t occupied() const {
        return _occupied;
    }

    void reserve(size_t n) {
        resize(n);
    }

    V* get(const K& key) {
        auto i = get_index(key);

        while (!_free[i]) {
            if (key == _keys[i]) {
                return &_values[i];
            }
            ++i;
            i &= _capacity - 1;
        }

        return nullptr;
    }

    void remove(const K& key) {
        auto i = get_index(key);

        while (!_free[i]) {
            if (key == _keys[i]) {
                _free[i] = 1;
                --_occupied;
                _values[i] = V{};
                _keys[i]   = K{};
                return;
            }
            ++i;
            i &= _capacity - 1;
        }
    }

    void insert(const K& key, V value) {
        resize_if_needed();

        auto i = get_index(key);

        while (!_free[i]) {
            if (key == _keys[i]) {
                _values[i] = value;
                return;
            }
            ++i;
            i &= _capacity - 1;
        }

        _keys[i]   = key;
        _values[i] = value;
        _free[i]   = 0;
        ++_occupied;
    }

    V& operator[](const K& key) {
        resize_if_needed();

        auto i = get_index(key);

        while (!_free[i]) {
            if (key == _keys[i]) {
                return _values[i];
            }
            ++i;
            i &= _capacity - 1;
        }

        _keys[i]   = key;
        _values[i] = V{};
        _free[i]   = 0;
        ++_occupied;

        return _values[i];
    }
};

#endif // !__OPEN_HASH_MAP_H__
