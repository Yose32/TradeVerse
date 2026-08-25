#ifndef TRADEVERSE_MIN_HEAP_HPP
#define TRADEVERSE_MIN_HEAP_HPP

#include <vector>
#include <functional>
#include <stdexcept>
#include <utility>

namespace tradeverse {
namespace dsa {

/**
 * @brief Custom Binary Min-Heap implementation.
 * 
 * Used for the Sell side of the Order Book: Lowest ask price gets top priority.
 * 
 * Time Complexity:
 * - Insert (push): O(log N)
 * - Extract Min (pop): O(log N)
 * - Peek Min (top): O(1)
 * - Arbitrary Remove: O(N) search + O(log N) heapify
 * Space Complexity: O(N)
 */
template <typename T, typename Comparator = std::greater<T>>
class MinHeap {
private:
    std::vector<T> data;
    Comparator compare; // Returns true if a > b (meaning b has higher priority for min-heap)

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
        size_t smallest = index;
        size_t left = leftChild(index);
        size_t right = rightChild(index);

        if (left < data.size() && compare(data[smallest], data[left])) {
            smallest = left;
        }
        if (right < data.size() && compare(data[smallest], data[right])) {
            smallest = right;
        }

        if (smallest != index) {
            std::swap(data[index], data[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    MinHeap() = default;
    explicit MinHeap(Comparator comp) : compare(comp) {}

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
            throw std::out_of_range("MinHeap is empty");
        }
        return data[0];
    }

    T& top() {
        if (data.empty()) {
            throw std::out_of_range("MinHeap is empty");
        }
        return data[0];
    }

    T pop() {
        if (data.empty()) {
            throw std::out_of_range("MinHeap is empty");
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
        MinHeap<T, Comparator> copy = *this;
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

#endif // TRADEVERSE_MIN_HEAP_HPP
