#ifndef TRADEVERSE_QUEUE_HPP
#define TRADEVERSE_QUEUE_HPP

#include <stdexcept>
#include <cstddef>
#include <utility>

namespace tradeverse {
namespace dsa {

/**
 * @brief Custom FIFO Queue implementation.
 * 
 * Used for:
 * - Incoming order ingestion buffer
 * - Market simulation event stream
 * - Breadth-First Search (BFS) frontier queue
 * 
 * Time Complexity:
 * - Enqueue: O(1)
 * - Dequeue: O(1)
 * - Front: O(1)
 * Space Complexity: O(N)
 */
template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& val) : data(val), next(nullptr) {}
        explicit Node(T&& val) : data(std::move(val)), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    size_t count;

public:
    Queue() : head(nullptr), tail(nullptr), count(0) {}

    ~Queue() {
        clear();
    }

    Queue(const Queue& other) : head(nullptr), tail(nullptr), count(0) {
        Node* curr = other.head;
        while (curr) {
            enqueue(curr->data);
            curr = curr->next;
        }
    }

    Queue& operator=(const Queue& other) {
        if (this != &other) {
            clear();
            Node* curr = other.head;
            while (curr) {
                enqueue(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    Queue(Queue&& other) noexcept : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            count = other.count;
            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }
        return *this;
    }

    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        ++count;
    }

    void enqueue(T&& value) {
        Node* newNode = new Node(std::move(value));
        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        ++count;
    }

    T dequeue() {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        Node* temp = head;
        T val = std::move(temp->data);
        head = head->next;
        if (!head) {
            tail = nullptr;
        }
        delete temp;
        --count;
        return val;
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return head->data;
    }

    T& front() {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return head->data;
    }

    bool empty() const { return count == 0; }
    size_t size() const { return count; }

    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        count = 0;
    }
};

} // namespace dsa
} // namespace tradeverse

#endif // TRADEVERSE_QUEUE_HPP
