#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <cmath>

#include "../include/dsa/MaxHeap.hpp"
#include "../include/dsa/MinHeap.hpp"
#include "../include/dsa/Queue.hpp"
#include "../include/dsa/Stack.hpp"
#include "../include/dsa/HashMap.hpp"
#include "../include/dsa/DoublyLinkedList.hpp"
#include "../include/dsa/Graph.hpp"
#include "../include/dsa/Sorting.hpp"
#include "../include/dsa/Searching.hpp"
#include "../include/models/Order.hpp"

using namespace tradeverse;
using namespace tradeverse::dsa;
using namespace tradeverse::models;

void testMaxHeap() {
    std::cout << "[TEST] Running MaxHeap tests..." << std::endl;
    MaxHeap<int> heap;
    assert(heap.empty());
    heap.push(20);
    heap.push(50);
    heap.push(30);
    heap.push(10);
    heap.push(60);

    assert(heap.size() == 5);
    assert(heap.top() == 60);
    assert(heap.pop() == 60);
    assert(heap.pop() == 50);
    assert(heap.pop() == 30);
    assert(heap.pop() == 20);
    assert(heap.pop() == 10);
    assert(heap.empty());

    // Test with Buy Order Comparator (highest price first, then earliest timestamp)
    MaxHeap<Order, BuyOrderComparator> buyBook;
    buyBook.push({"1", "u1", "AAPL", OrderType::LIMIT, OrderSide::BUY, 10, 0, 250.0, OrderStatus::PENDING, 1000});
    buyBook.push({"2", "u2", "AAPL", OrderType::LIMIT, OrderSide::BUY, 20, 0, 255.0, OrderStatus::PENDING, 1005});
    buyBook.push({"3", "u3", "AAPL", OrderType::LIMIT, OrderSide::BUY, 15, 0, 255.0, OrderStatus::PENDING, 1002}); // same price, earlier time
    buyBook.push({"4", "u4", "AAPL", OrderType::LIMIT, OrderSide::BUY, 30, 0, 248.0, OrderStatus::PENDING, 1001});

    assert(buyBook.size() == 4);
    Order top1 = buyBook.pop();
    assert(top1.orderId == "3"); // 255.0 @ t=1002
    Order top2 = buyBook.pop();
    assert(top2.orderId == "2"); // 255.0 @ t=1005
    Order top3 = buyBook.pop();
    assert(top3.orderId == "1"); // 250.0
    Order top4 = buyBook.pop();
    assert(top4.orderId == "4"); // 248.0
    assert(buyBook.empty());
    std::cout << "[PASS] MaxHeap tests passed!" << std::endl;
}

void testMinHeap() {
    std::cout << "[TEST] Running MinHeap tests..." << std::endl;
    MinHeap<int> heap;
    heap.push(50);
    heap.push(20);
    heap.push(80);
    heap.push(10);
    assert(heap.top() == 10);
    assert(heap.pop() == 10);
    assert(heap.pop() == 20);
    assert(heap.pop() == 50);
    assert(heap.pop() == 80);

    // Test with Sell Order Comparator (lowest ask first, then earliest timestamp)
    MinHeap<Order, SellOrderComparator> sellBook;
    sellBook.push({"1", "u1", "AAPL", OrderType::LIMIT, OrderSide::SELL, 10, 0, 260.0, OrderStatus::PENDING, 1000});
    sellBook.push({"2", "u2", "AAPL", OrderType::LIMIT, OrderSide::SELL, 20, 0, 252.0, OrderStatus::PENDING, 1005});
    sellBook.push({"3", "u3", "AAPL", OrderType::LIMIT, OrderSide::SELL, 15, 0, 252.0, OrderStatus::PENDING, 1001}); // same price, earlier time
    sellBook.push({"4", "u4", "AAPL", OrderType::LIMIT, OrderSide::SELL, 30, 0, 258.0, OrderStatus::PENDING, 1002});

    assert(sellBook.pop().orderId == "3"); // 252.0 @ t=1001
    assert(sellBook.pop().orderId == "2"); // 252.0 @ t=1005
    assert(sellBook.pop().orderId == "4"); // 258.0
    assert(sellBook.pop().orderId == "1"); // 260.0
    std::cout << "[PASS] MinHeap tests passed!" << std::endl;
}

void testQueue() {
    std::cout << "[TEST] Running Queue tests..." << std::endl;
    Queue<std::string> q;
    assert(q.empty());
    q.enqueue("order1");
    q.enqueue("order2");
    q.enqueue("order3");
    assert(q.size() == 3);
    assert(q.front() == "order1");
    assert(q.dequeue() == "order1");
    assert(q.dequeue() == "order2");
    assert(q.dequeue() == "order3");
    assert(q.empty());
    std::cout << "[PASS] Queue tests passed!" << std::endl;
}

void testStack() {
    std::cout << "[TEST] Running Stack tests..." << std::endl;
    Stack<std::string> st;
    assert(st.empty());
    st.push("action1");
    st.push("action2");
    st.push("action3");
    assert(st.size() == 3);
    assert(st.top() == "action3");
    assert(st.pop() == "action3");
    assert(st.pop() == "action2");
    assert(st.pop() == "action1");
    assert(st.empty());
    std::cout << "[PASS] Stack tests passed!" << std::endl;
}

void testHashMap() {
    std::cout << "[TEST] Running HashMap tests..." << std::endl;
    HashMap<std::string, double> priceMap;
    assert(priceMap.empty());
    priceMap.put("AAPL", 250.5);
    priceMap.put("MSFT", 420.0);
    priceMap.put("NVDA", 130.2);

    assert(priceMap.size() == 3);
    assert(priceMap.contains("AAPL"));
    assert(!priceMap.contains("TSLA"));

    double p = 0;
    assert(priceMap.get("MSFT", p) && std::abs(p - 420.0) < 1e-6);

    // Update
    priceMap.put("AAPL", 255.0);
    assert(priceMap.get("AAPL", p) && std::abs(p - 255.0) < 1e-6);

    // Dynamic rehashing test
    for (int i = 0; i < 50; ++i) {
        priceMap.put("SYM" + std::to_string(i), static_cast<double>(i * 10));
    }
    assert(priceMap.size() == 53);
    assert(priceMap.get("SYM25", p) && std::abs(p - 250.0) < 1e-6);

    // Erase
    assert(priceMap.erase("SYM25"));
    assert(!priceMap.contains("SYM25"));
    assert(priceMap.size() == 52);
    std::cout << "[PASS] HashMap tests passed!" << std::endl;
}

void testDoublyLinkedList() {
    std::cout << "[TEST] Running DoublyLinkedList tests..." << std::endl;
    DoublyLinkedList<int> dll;
    assert(dll.empty());
    dll.pushBack(10);
    dll.pushBack(20);
    dll.pushFront(5);
    // Sequence: 5, 10, 20
    assert(dll.size() == 3);
    assert(dll.popFront() == 5);
    assert(dll.popBack() == 20);
    assert(dll.popBack() == 10);
    assert(dll.empty());
    std::cout << "[PASS] DoublyLinkedList tests passed!" << std::endl;
}

void testGraph() {
    std::cout << "[TEST] Running Graph (BFS, DFS, Dijkstra) tests..." << std::endl;
    Graph g;
    g.addEdge("Semiconductors", "NVDA", 1.0, "contains");
    g.addEdge("Semiconductors", "AMD", 1.0, "contains");
    g.addEdge("Semiconductors", "TSMC", 1.5, "foundry");
    g.addEdge("NVDA", "AI_Sector", 1.2, "drives");
    g.addEdge("AI_Sector", "MSFT", 1.0, "cloud");
    g.addEdge("AI_Sector", "GOOGL", 1.0, "search");

    // BFS
    auto bfsOrder = g.bfs("Semiconductors");
    assert(!bfsOrder.empty());
    assert(bfsOrder[0] == "Semiconductors");

    // DFS
    auto dfsOrder = g.dfs("Semiconductors");
    assert(!dfsOrder.empty());
    assert(dfsOrder[0] == "Semiconductors");

    // Dijkstra
    auto dijkstraRes = g.dijkstra("Semiconductors");
    assert(dijkstraRes.distances["Semiconductors"] == 0.0);
    assert(dijkstraRes.distances["NVDA"] == 1.0);
    assert(dijkstraRes.distances["AI_Sector"] == 2.2);
    assert(dijkstraRes.distances["MSFT"] == 3.2);

    auto path = dijkstraRes.getPathTo("MSFT");
    // Path should be: Semiconductors -> NVDA -> AI_Sector -> MSFT
    assert(path.size() == 4);
    assert(path[0] == "Semiconductors" && path[1] == "NVDA" && path[2] == "AI_Sector" && path[3] == "MSFT");

    // Shock propagation
    auto shockMap = g.propagateShock("Semiconductors", -0.10);
    assert(shockMap["Semiconductors"] < 0);
    assert(std::abs(shockMap["NVDA"]) > std::abs(shockMap["MSFT"])); // closer node suffers stronger impact

    std::cout << "[PASS] Graph tests passed!" << std::endl;
}

void testSortingAndSearching() {
    std::cout << "[TEST] Running Sorting and Searching tests..." << std::endl;
    std::vector<int> arr = {64, 25, 12, 22, 11, 90, 45, 1, 88};
    std::vector<int> arrMerge = arr;
    std::vector<int> arrQuick = arr;

    Sorting::mergeSort(arrMerge);
    Sorting::quickSort(arrQuick);

    std::vector<int> expected = {1, 11, 12, 22, 25, 45, 64, 88, 90};
    assert(arrMerge == expected);
    assert(arrQuick == expected);

    // Binary search
    assert(Searching::binarySearch(arrMerge, 25) == 4);
    assert(Searching::binarySearch(arrMerge, 1) == 0);
    assert(Searching::binarySearch(arrMerge, 90) == 8);
    assert(Searching::binarySearch(arrMerge, 999) == -1);

    std::cout << "[PASS] Sorting and Searching tests passed!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " TradeVerse DSA Core Engine Test Suite  " << std::endl;
    std::cout << "========================================" << std::endl;

    testMaxHeap();
    testMinHeap();
    testQueue();
    testStack();
    testHashMap();
    testDoublyLinkedList();
    testGraph();
    testSortingAndSearching();

    std::cout << "\n>>> ALL C++ DSA CORE TESTS PASSED SUCCESSFULLY! <<<" << std::endl;
    return 0;
}
