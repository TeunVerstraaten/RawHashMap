#ifndef __RAW_HASH_MAP_H__
#define __RAW_HASH_MAP_H__

#include <bit>
#include <cassert>
#include <cstdint>
#include <new>
#include <vector>

template <typename K, typename V>
class RawHashMap {
    size_t               _capacity;
    size_t               _occupied;
    double               _resize_threshold;
    K*                   _keys;
    V*                   _values;
    std::vector<uint8_t> _free;

    void resize(size_t new_size) {
        auto rounded_new_size = std::__bit_ceil(new_size);
        auto new_hm           = RawHashMap<K, V>(rounded_new_size, _resize_threshold);
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

        new (&_keys[i]) K(std::move(key));
        new (&_values[i]) V(std::move(value));
        _free[i] = 0;
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
    RawHashMap(size_t size_hint = 32, double resize_threshold = 0.7)
        : _capacity(std::__bit_ceil(size_hint)),
          _occupied(0),
          _resize_threshold(resize_threshold),
          _keys(static_cast<K*>(::operator new(sizeof(K) * _capacity, std::align_val_t{alignof(K)}))),
          _values(static_cast<V*>(::operator new(sizeof(V) * _capacity, std::align_val_t{alignof(V)}))),
          _free(_capacity, 1) {
    }

    ~RawHashMap() {
        for (size_t i = 0; i != _capacity; ++i) {
            if (!_free[i]) {
                _keys[i].~K();
                _values[i].~V();
            }
        }
        ::operator delete(_keys);
        ::operator delete(_values);
    }

    size_t capacity() const {
        return _capacity;
    }

    size_t occupied() const {
        return _occupied;
    }

    void reserve(size_t n) {
        if (n > _capacity) {
            resize(n);
        }
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

    bool remove(const K& key) {
        auto i = get_index(key);

        while (!_free[i]) {
            if (key == _keys[i]) {
                _free[i] = 1;
                --_occupied;
                _values[i].~V();
                _keys[i].~K();
                return true;
            }
            ++i;
            i &= _capacity - 1;
        }
        return false;
    }

    bool insert(const K& key, const V& value) {
        resize_if_needed();

        auto i = get_index(key);

        while (!_free[i]) {
            if (key == _keys[i]) {
                return false;
            }
            ++i;
            i &= _capacity - 1;
        }

        new (&_keys[i]) K(key);
        new (&_values[i]) V(value);
        _free[i] = 0;
        ++_occupied;
        return true;
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

        new (&_keys[i]) K(std::move(key));
        new (&_values[i]) V{};
        _free[i] = 0;
        ++_occupied;

        return _values[i];
    }
};

#endif // !__RAW_HASH_MAP_H__
