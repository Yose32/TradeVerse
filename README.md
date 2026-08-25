# TradeVerse — Full-Stack C++ DSA Trading Simulator

<div align="center">

![TradeVerse Banner](https://img.shields.io/badge/TradeVerse-Virtual%20Trading%20Simulator-3b82f6?style=for-the-badge&logo=trendingup)
![Engine](https://img.shields.io/badge/Core%20Engine-C%2B%2B20%20DSA-10b981?style=for-the-badge&logo=c%2B%2B)
![Persistence](https://img.shields.io/badge/Database-SQLite3%20WAL-8b5cf6?style=for-the-badge&logo=sqlite)
![Frontend](https://img.shields.io/badge/Frontend-React%20%7C%20TypeScript%20%7C%20Vite%20%7C%20Tailwind-f59e0b?style=for-the-badge&logo=react)

**A high-performance virtual paper trading simulator engineered from the ground up as a premier College Data Structures & Algorithms project.**

</div>

---

## 🌟 1. Project Overview

TradeVerse is an end-to-end simulated financial exchange platform. Every user starts with **₹10,00,000 in virtual capital** to trade simulated equities.

> **Academic Invariant**: All core trading logic, order matching, priority queues, binary heaps, graph shockwave contagion, hash tables, and leaderboard sorting algorithms are implemented from scratch in **C++20**. The React + TypeScript frontend serves strictly as the presentation layer.

---

## 🏗️ 2. System Architecture

```
React + TypeScript + Vite + Tailwind CSS (Port 3000)
                  ↕ (JSON REST API)
       C++ HTTP Server (Port 8080)
                  ↕
    C++ Trading & Matching Engine
                  ↕
  Custom C++ Data Structures Layer:
  ├── MaxHeap<Order> (Buy Bids Priority Queue)
  ├── MinHeap<Order> (Sell Asks Priority Queue)
  ├── Queue<Order> (Arrival FIFO Buffer)
  ├── Stack<Action> (Undo / Action Log)
  ├── HashMap<Key, Value> (O(1) Symbol & Token Cache)
  ├── DoublyLinkedList<Trade> (Trade Feed & Price Buckets)
  ├── Graph (Sectors, Supply-Chains & BFS/DFS/Dijkstra)
  └── Sorting (Merge Sort for Leaderboard, Quick Sort for Gainers)
                  ↕
       SQLite3 Database (tradeverse.db)
```

---

## 📊 3. Data Structures & Algorithms (DSA) Matrix

| Data Structure / Algorithm | TradeVerse Feature | Time Complexity | Space Complexity | C++ Implementation File | Viva Justification |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Binary Max-Heap** | Buy Side Order Book | $O(\log N)$ push/pop, $O(1)$ peek | $O(N)$ | `backend/include/dsa/MaxHeap.hpp` | Guarantees the highest bid price is prioritized for matching. |
| **Binary Min-Heap** | Sell Side Order Book | $O(\log N)$ push/pop, $O(1)$ peek | $O(N)$ | `backend/include/dsa/MinHeap.hpp` | Guarantees the lowest selling ask is matched first. |
| **Price-Time Matching** | FIFO Order Matching Engine | $O(K \log N)$ | $O(N)$ | `backend/include/trading/MatchingEngine.hpp` | Matches orders across bid-ask spread; breaks price ties chronologically. |
| **FIFO Queue** | Incoming Order Stream & BFS | $O(1)$ enqueue/dequeue | $O(N)$ | `backend/include/dsa/Queue.hpp` | Preserves deterministic order arrival sequence. |
| **LIFO Stack** | Action Rollback & DFS Backtrack | $O(1)$ push/pop | $O(N)$ | `backend/include/dsa/Stack.hpp` | Supports immediate cancellation and DFS recursion tracking. |
| **Separate Chaining Hash Table** | Symbol -> Stock & Session Cache | $O(1)$ average lookup | $O(N + M)$ | `backend/include/dsa/HashMap.hpp` | Instant constant-time metadata resolution with dynamic rehashing. |
| **Doubly Linked List** | Chronological Trade Ticker | $O(1)$ push/pop/delete | $O(N)$ | `backend/include/dsa/DoublyLinkedList.hpp` | Constant-time node removal and insertion for price buckets. |
| **Adjacency List Graph** | Market Sectors & Supply Chains | $O(V + E)$ representation | $O(V + E)$ | `backend/include/dsa/Graph.hpp` | Models inter-stock correlations and macroeconomic contagion. |
| **Dijkstra's Shortest Path** | Minimum Shock Resistance Path | $O((V + E) \log V)$ | $O(V)$ | `backend/include/dsa/Graph.hpp` | Computes decay factors as macro shocks propagate across sectors. |
| **Merge Sort** | Stable Leaderboard Ranking | $O(N \log N)$ stable | $O(N)$ | `backend/include/dsa/Sorting.hpp` | Produces deterministic ranking of participants by net worth. |
| **Quick Sort** | Top Market Gainers / Losers | $O(N \log N)$ average | $O(\log N)$ | `backend/include/dsa/Sorting.hpp` | In-place partitioning for fast live ticker sorting. |
| **Binary Search** | Historical Price Points Lookup | $O(\log N)$ | $O(1)$ | `backend/include/dsa/Searching.hpp` | Rapid logarithmic search across sorted price ranges. |

---

## 🚀 4. Installation & Running Instructions

### Prerequisites
- Modern 64-bit C++ Compiler (GCC 14+ / Clang / MSVC)
- Node.js 18+ and npm

### 1. Build and Run the C++ Backend Engine
```bash
# Navigate to backend directory
cd backend

# Compile the C++20 engine with SQLite3 and Winsock2
gcc -c src/database/sqlite3.c -O2 -o src/database/sqlite3.o
g++ -std=c++20 -O2 src/main.cpp src/database/sqlite3.o -lws2_32 -o tradeverse_server.exe

# Start the C++ backend server (runs on port 8080)
./tradeverse_server.exe
```

### 2. Run the C++ Test Suite
```bash
# Compile and run the DSA unit test suite
g++ -std=c++20 -O2 tests/test_dsa.cpp -o tests/test_dsa.exe
./tests/test_dsa.exe

# Compile and run the matching engine test suite
g++ -std=c++20 -O2 tests/test_matching_engine.cpp -o tests/test_matching_engine.exe
./tests/test_matching_engine.exe
```

### 3. Start the Frontend Web Application
```bash
# Navigate to frontend directory
cd frontend

# Install dependencies
npm install

# Start Vite development server (runs on port 3000)
npm run dev
```

Visit **http://localhost:3000** in your browser.

---

## 🔑 5. Pre-Seeded Demo Accounts

Every demo account starts with **₹10,00,000 virtual capital**:

| Username | Email | Password | Role |
| :--- | :--- | :--- | :--- |
| `demo_trader` | `demo@tradeverse.com` | `DemoPass123!` | Demo Participant |
| `alex_invest` | `alex@tradeverse.com` | `AlexPass123!` | Competitor |
| `sarah_quant` | `sarah@tradeverse.com` | `SarahPass123!` | Competitor |
| `rahul_alpha` | `rahul@tradeverse.com` | `RahulPass123!` | Competitor |
| `admin` | `admin@tradeverse.com` | `AdminTradeVerse2026!` | Simulation Admin |

---

## 🧪 6. DSA Laboratory Features

Navigate to the **DSA Lab** page in the top navigation bar to interact with:
1. **Binary Heap Visualizer**: Step through `push` (heapify-up) and `pop` (heapify-down) with animated node swaps.
2. **Market Sector Graph**: Select source and target sectors to execute **Dijkstra**, **BFS**, or **DFS** and view the step-by-step resistance calculations.
3. **Sorting Suite**: Run side-by-side **Merge Sort** and **Quick Sort** with comparison counts and subarray partition highlights.
4. **Complexity Table**: Comprehensive mathematical proofs and viva examination references.

---

## 🛡️ 7. Security & Persistence

- Passwords salted and hashed with standalone **SHA-256**.
- Token-based authenticated sessions with bearer token headers.
- **SQLite3** persistence with Write-Ahead Logging (WAL) and parameterized queries.