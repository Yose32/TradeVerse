#ifndef TRADEVERSE_MAX_HEAP_HPP
#define TRADEVERSE_MAX_HEAP_HPP

#include <vector>
#include <functional>
#include <stdexcept>
#include <utility>

namespace tradeverse {
namespace dsa {

/**
 * @brief Custom Binary Max-Heap implementation.
 * 
 * Used for the Buy side of the Order Book: Highest bid price gets top priority.
 * 
 * Time Complexity:
 * - Insert (push): O(log N)
 * - Extract Max (pop): O(log N)
 * - Peek Max (top): O(1)
 * - Arbitrary Remove: O(N) search + O(log N) heapify
 * Space Complexity: O(N)
 */
template <typename T, typename Comparator = std::less<T>>
class MaxHeap {
private:
    std::vector<T> data;
    Comparator compare; // Returns true if a < b (meaning b has higher priority)

    size_t parent(size_t index) const { return (index - 1) / 2; }
    size_t leftChild(size_t index) const { return 2 * index + 1; }
    size_t rightChild(size_t index) const { return 2 * index + 2; }

    void heapifyUp(size_t index) {
        while (index > 0 && compare(data[parent(index)], data[index])) {
            std::swap(data[parent(index)], data[index]);
            index = parent(index);
        }
    }

    void heapifyDown(size_t index) {
        size_t largest = index;
        size_t left = leftChild(index);
        size_t right = rightChild(index);

        if (left < data.size() && compare(data[largest], data[left])) {
            largest = left;
        }
        if (right < data.size() && compare(data[largest], data[right])) {
            largest = right;
        }

        if (largest != index) {
            std::swap(data[index], data[largest]);
            heapifyDown(largest);
        }
    }

public:
    MaxHeap() = default;
    explicit MaxHeap(Comparator comp) : compare(comp) {}

    void push(const T& value) {
        data.push_back(value);
        heapifyUp(data.size() - 1);
    }

    void push(T&& value) {
        data.push_back(std::move(value));
        heapifyUp(data.size() - 1);
    }

    const T& top() const {
        if (data.empty()) {
            throw std::out_of_range("MaxHeap is empty");
        }
        return data[0];
    }

    T& top() {
        if (data.empty()) {
            throw std::out_of_range("MaxHeap is empty");
        }
        return data[0];
    }

    T pop() {
        if (data.empty()) {
            throw std::out_of_range("MaxHeap is empty");
        }
        T root = data[0];
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) {
            heapifyDown(0);
        }
        return root;
    }

    bool removeIf(std::function<bool(const T&)> predicate) {
        for (size_t i = 0; i < data.size(); ++i) {
            if (predicate(data[i])) {
                data[i] = data.back();
                data.pop_back();
                if (i < data.size()) {
                    heapifyUp(i);
                    heapifyDown(i);
                }
                return true;
            }
        }
        return false;
    }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
    void clear() { data.clear(); }
    const std::vector<T>& rawData() const { return data; }

    std::vector<T> toSortedVector() const {
        MaxHeap<T, Comparator> copy = *this;
        std::vector<T> result;
        result.reserve(copy.size());
        while (!copy.empty()) {
            result.push_back(copy.pop());
        }
        return result;
    }
};

} // namespace dsa
} // namespace tradeverse

#endif // TRADEVERSE_MAX_HEAP_HPP
