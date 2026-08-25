#ifndef TRADEVERSE_DOUBLY_LINKED_LIST_HPP
#define TRADEVERSE_DOUBLY_LINKED_LIST_HPP

#include <stdexcept>
#include <vector>
#include <utility>
#include <cstddef>

namespace tradeverse {
namespace dsa {

/**
 * @brief Custom Doubly Linked List implementation.
 * 
 * Used for:
 * - Chronological trade history stream
 * - Order book price-level FIFO order chains (constant time insertion and deletion)
 * - Recent user transaction records
 * 
 * Time Complexity:
 * - Push Front / Push Back: O(1)
 * - Pop Front / Pop Back: O(1)
 * - Node Deletion: O(1)
 * - Linear Search / Traversal: O(N)
 * Space Complexity: O(N)
 */
template <typename T>
class DoublyLinkedList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        explicit Node(const T& val) : data(val), prev(nullptr), next(nullptr) {}
        explicit Node(T&& val) : data(std::move(val)), prev(nullptr), next(nullptr) {}
    };

private:
    Node* head;
    Node* tail;
    size_t count;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~DoublyLinkedList() {
        clear();
    }

    DoublyLinkedList(const DoublyLinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        Node* curr = other.head;
        while (curr) {
            pushBack(curr->data);
            curr = curr->next;
        }
    }

    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
        if (this != &other) {
            clear();
            Node* curr = other.head;
            while (curr) {
                pushBack(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    DoublyLinkedList(DoublyLinkedList&& other) noexcept : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
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

    Node* pushBack(const T& value) {
        Node* newNode = new Node(value);
        if (!tail) {
            head = tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        ++count;
        return newNode;
    }

    Node* pushBack(T&& value) {
        Node* newNode = new Node(std::move(value));
        if (!tail) {
            head = tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        ++count;
        return newNode;
    }

    Node* pushFront(const T& value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        ++count;
        return newNode;
    }

    T popFront() {
        if (empty()) {
            throw std::out_of_range("DoublyLinkedList is empty");
        }
        Node* temp = head;
        T val = std::move(temp->data);
        head = head->next;
        if (head) {
            head->prev = nullptr;
        } else {
            tail = nullptr;
        }
        delete temp;
        --count;
        return val;
    }

    T popBack() {
        if (empty()) {
            throw std::out_of_range("DoublyLinkedList is empty");
        }
        Node* temp = tail;
        T val = std::move(temp->data);
        tail = tail->prev;
        if (tail) {
            tail->next = nullptr;
        } else {
            head = nullptr;
        }
        delete temp;
        --count;
        return val;
    }

    void removeNode(Node* node) {
        if (!node) return;
        if (node == head) {
            popFront();
            return;
        }
        if (node == tail) {
            popBack();
            return;
        }
        node->prev->next = node->next;
        node->next->prev = node->prev;
        delete node;
        --count;
    }

    Node* getHead() const { return head; }
    Node* getTail() const { return tail; }

    bool empty() const { return count == 0; }
    size_t size() const { return count; }

    void clear() {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
        head = tail = nullptr;
        count = 0;
    }

    std::vector<T> toVector() const {
        std::vector<T> result;
        result.reserve(count);
        Node* curr = head;
        while (curr) {
            result.push_back(curr->data);
            curr = curr->next;
        }
        return result;
    }
};

} // namespace dsa
} // namespace tradeverse

#endif // TRADEVERSE_DOUBLY_LINKED_LIST_HPP
