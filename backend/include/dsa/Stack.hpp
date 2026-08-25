#ifndef TRADEVERSE_STACK_HPP
#define TRADEVERSE_STACK_HPP

#include <stdexcept>
#include <cstddef>
#include <utility>
#include <vector>

namespace tradeverse {
namespace dsa {

/**
 * @brief Custom LIFO Stack implementation.
 * 
 * Used for:
 * - User action undo history (undoing simulated orders/actions)
 * - Traversal backtracking in Depth-First Search (DFS)
 * - Navigation history and state snapshots
 * 
 * Time Complexity:
 * - Push: O(1) amortized
 * - Pop: O(1)
 * - Top: O(1)
 * Space Complexity: O(N)
 */
template <typename T>
class Stack {
private:
    std::vector<T> elements;

public:
    Stack() = default;

    void push(const T& value) {
        elements.push_back(value);
    }

    void push(T&& value) {
        elements.push_back(std::move(value));
    }

    T pop() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        T topVal = std::move(elements.back());
        elements.pop_back();
        return topVal;
    }

    const T& top() const {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return elements.back();
    }

    T& top() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return elements.back();
    }

    bool empty() const { return elements.empty(); }
    size_t size() const { return elements.size(); }
    void clear() { elements.clear(); }

    const std::vector<T>& rawData() const { return elements; }
};

} // namespace dsa
} // namespace tradeverse

#endif // TRADEVERSE_STACK_HPP
