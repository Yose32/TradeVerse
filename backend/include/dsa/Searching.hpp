#ifndef TRADEVERSE_SEARCHING_HPP
#define TRADEVERSE_SEARCHING_HPP

#include <vector>
#include <string>

namespace tradeverse {
namespace dsa {

/**
 * @brief Search Step Log for educational visualization in DSA Lab.
 */
struct SearchStepLog {
    std::string algorithm;
    int target;
    int low;
    int mid;
    int high;
    int midValue;
    bool found;
    std::string description;
};

/**
 * @brief Custom Searching Algorithms Suite.
 * 
 * Includes:
 * - Binary Search: O(log N) search on sorted stock arrays and historical price points.
 */
class Searching {
public:
    template <typename T>
    static int binarySearch(const std::vector<T>& arr, const T& target) {
        int low = 0;
        int high = static_cast<int>(arr.size()) - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (arr[mid] == target) {
                return mid;
            } else if (arr[mid] < target) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return -1;
    }

    static std::vector<SearchStepLog> traceBinarySearch(const std::vector<int>& arr, int target) {
        std::vector<SearchStepLog> logs;
        int low = 0;
        int high = static_cast<int>(arr.size()) - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            SearchStepLog log;
            log.algorithm = "Binary Search";
            log.target = target;
            log.low = low;
            log.mid = mid;
            log.high = high;
            log.midValue = arr[mid];

            if (arr[mid] == target) {
                log.found = true;
                log.description = "Target " + std::to_string(target) + " found at index " + std::to_string(mid);
                logs.push_back(log);
                return logs;
            } else if (arr[mid] < target) {
                log.found = false;
                log.description = "Value " + std::to_string(arr[mid]) + " < " + std::to_string(target) + "; searching right half [" + std::to_string(mid + 1) + ".." + std::to_string(high) + "]";
                logs.push_back(log);
                low = mid + 1;
            } else {
                log.found = false;
                log.description = "Value " + std::to_string(arr[mid]) + " > " + std::to_string(target) + "; searching left half [" + std::to_string(low) + ".." + std::to_string(mid - 1) + "]";
                logs.push_back(log);
                high = mid - 1;
            }
        }

        SearchStepLog notFound;
        notFound.algorithm = "Binary Search";
        notFound.target = target;
        notFound.low = low;
        notFound.mid = -1;
        notFound.high = high;
        notFound.midValue = -1;
        notFound.found = false;
        notFound.description = "Target " + std::to_string(target) + " not found in array.";
        logs.push_back(notFound);

        return logs;
    }
};

} // namespace dsa
} // namespace tradeverse

#endif // TRADEVERSE_SEARCHING_HPP
