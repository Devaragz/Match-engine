# High-Performance Matching Engine (Limit Order Book)

A concurrent trade matching engine built entirely in C++. This system simulates the backend architecture of a high-frequency trading (HFT) exchange, processing thousands of simultaneous network orders with microsecond latency.

## Core Architecture & Features

* **O(1) Execution & Modifications:** Engineered a custom data structure combining `std::unordered_map` with stable `std::list` iterators to achieve constant time complexity for order lookups, cancellations, and modifications.
* **Multithreaded Processing:** Designed a thread-safe architecture utilizing a dedicated worker thread and a mutex-protected concurrent queue to eliminate main-thread blocking during heavy load.
* **TCP Socket Networking:** Built a custom asynchronous TCP server using `winsock2` to deserialize live network payloads from multiple concurrent clients.
* **Real-Time Analytics:** Implemented live Volume-Weighted Average Price (VWAP) and total volume calculations.
* **Data Persistence:** Integrated an append-only disk logging mechanism to record executed trades for auditing and crash recovery.

## Tech Stack
* **Language:** C++17
* **Networking:** Winsock2 (Windows API) / POSIX Sockets
* **Concurrency:** `std::thread`, `std::mutex`, `std::condition_variable`
* **Testing:** Python 3 (Socket programming for load testing)

## How to Run

### 1. Compile the Engine
Use the following command to compile the C++ source files (MinGW/Windows):
```powershell
g++ src/*.cpp -o engine -lws2_32 -std=c++17
./engine
