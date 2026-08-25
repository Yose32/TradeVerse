#ifndef TRADEVERSE_HASH_MAP_HPP
#define TRADEVERSE_HASH_MAP_HPP

#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <cstddef>
#include <functional>

namespace tradeverse {
namespace dsa {

/**
 * @brief Custom Hash Table with Separate Chaining and Dynamic Rehashing.
 * 
 * Used for:
 * - Symbol -> StockInfo fast lookup
 * - SessionToken -> UserSession authentication cache
 * - UserID -> Portfolio metadata
 * 
 * Time Complexity:
 * - Lookup (get): O(1) average, O(N) worst case
 * - Insert (put): O(1) average, O(N) worst case
 * - Delete (remove): O(1) average, O(N) worst case
 * Space Complexity: O(N + M) where M is bucket count
 */
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class HashMap {
private:
    struct Entry {
        Key key;
        Value value;
        Entry* next;
        Entry(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
        Entry(Key&& k, Value&& v) : key(std::move(k)), value(std::move(v)), next(nullptr) {}
    };

    std::vector<Entry*> buckets;
    size_t numElements;
    float maxLoadFactor;
    Hash hasher;

    static const size_t DEFAULT_INITIAL_CAPACITY = 16;
    static constexpr float DEFAULT_MAX_LOAD_FACTOR = 0.75f;

    size_t getBucketIndex(const Key& key, size_t capacity) const {
        return hasher(key) % capacity;
    }

    void rehash(size_t newCapacity) {
        std::vector<Entry*> newBuckets(newCapacity, nullptr);
        for (size_t i = 0; i < buckets.size(); ++i) {
            Entry* curr = buckets[i];
            while (curr) {
                Entry* next = curr->next;
                size_t newIndex = getBucketIndex(curr->key, newCapacity);
                curr->next = newBuckets[newIndex];
                newBuckets[newIndex] = curr;
                curr = next;
            }
        }
        buckets = std::move(newBuckets);
    }

public:
    HashMap(size_t initialCapacity = DEFAULT_INITIAL_CAPACITY, float loadFactor = DEFAULT_MAX_LOAD_FACTOR)
        : buckets(initialCapacity, nullptr), numElements(0), maxLoadFactor(loadFactor) {}

    ~HashMap() {
        clear();
    }

    HashMap(const HashMap& other)
        : buckets(other.buckets.size(), nullptr), numElements(0), maxLoadFactor(other.maxLoadFactor) {
        for (size_t i = 0; i < other.buckets.size(); ++i) {
            Entry* curr = other.buckets[i];
            while (curr) {
                put(curr->key, curr->value);
                curr = curr->next;
            }
        }
    }

    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear();
            buckets.resize(other.buckets.size(), nullptr);
            maxLoadFactor = other.maxLoadFactor;
            for (size_t i = 0; i < other.buckets.size(); ++i) {
                Entry* curr = other.buckets[i];
                while (curr) {
                    put(curr->key, curr->value);
                    curr = curr->next;
                }
            }
        }
        return *this;
    }

    HashMap(HashMap&& other) noexcept
        : buckets(std::move(other.buckets)), numElements(other.numElements), maxLoadFactor(other.maxLoadFactor) {
        other.numElements = 0;
    }

    HashMap& operator=(HashMap&& other) noexcept {
        if (this != &other) {
            clear();
            buckets = std::move(other.buckets);
            numElements = other.numElements;
            maxLoadFactor = other.maxLoadFactor;
            other.numElements = 0;
        }
        return *this;
    }

    void put(const Key& key, const Value& value) {
        if (static_cast<float>(numElements + 1) / buckets.size() > maxLoadFactor) {
            rehash(buckets.size() * 2);
        }

        size_t index = getBucketIndex(key, buckets.size());
        Entry* curr = buckets[index];
        while (curr) {
            if (curr->key == key) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        Entry* newEntry = new Entry(key, value);
        newEntry->next = buckets[index];
        buckets[index] = newEntry;
        ++numElements;
    }

    bool get(const Key& key, Value& outValue) const {
        size_t index = getBucketIndex(key, buckets.size());
        Entry* curr = buckets[index];
        while (curr) {
            if (curr->key == key) {
                outValue = curr->value;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    Value* find(const Key& key) {
        size_t index = getBucketIndex(key, buckets.size());
        Entry* curr = buckets[index];
        while (curr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    const Value* find(const Key& key) const {
        size_t index = getBucketIndex(key, buckets.size());
        Entry* curr = buckets[index];
        while (curr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    bool contains(const Key& key) const {
        return find(key) != nullptr;
    }

    Value& operator[](const Key& key) {
        Value* val = find(key);
        if (val) {
            return *val;
        }
        put(key, Value{});
        return *find(key);
    }

    bool erase(const Key& key) {
        size_t index = getBucketIndex(key, buckets.size());
        Entry* curr = buckets[index];
        Entry* prev = nullptr;

        while (curr) {
            if (curr->key == key) {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    buckets[index] = curr->next;
                }
                delete curr;
                --numElements;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    void clear() {
        for (size_t i = 0; i < buckets.size(); ++i) {
            Entry* curr = buckets[i];
            while (curr) {
                Entry* temp = curr;
                curr = curr->next;
                delete temp;
            }
            buckets[i] = nullptr;
        }
        numElements = 0;
    }

    size_t size() const { return numElements; }
    bool empty() const { return numElements == 0; }
    size_t bucketCount() const { return buckets.size(); }
    float loadFactor() const { return static_cast<float>(numElements) / buckets.size(); }

    std::vector<Key> keys() const {
        std::vector<Key> result;
        result.reserve(numElements);
        for (size_t i = 0; i < buckets.size(); ++i) {
            Entry* curr = buckets[i];
            while (curr) {
                result.push_back(curr->key);
                curr = curr->next;
            }
        }
        return result;
    }

    std::vector<std::pair<Key, Value>> entries() const {
        std::vector<std::pair<Key, Value>> result;
        result.reserve(numElements);
        for (size_t i = 0; i < buckets.size(); ++i) {
            Entry* curr = buckets[i];
            while (curr) {
                result.emplace_back(curr->key, curr->value);
                curr = curr->next;
            }
        }
        return result;
    }
};

} // namespace dsa
} // namespace tradeverse

#endif // TRADEVERSE_HASH_MAP_HPP
