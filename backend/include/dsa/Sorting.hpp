#ifndef TRADEVERSE_SORTING_HPP
#define TRADEVERSE_SORTING_HPP

#include <vector>
#include <functional>
#include <string>

namespace tradeverse {
namespace dsa {

/**
 * @brief Sorting Step Log for educational visualization in DSA Lab.
 */
struct SortStepLog {
    std::string algorithm;
    std::vector<int> arrayState;
    std::vector<size_t> highlightedIndices;
    std::string description;
};

/**
 * @brief Custom Sorting Algorithms Suite.
 * 
 * Includes:
 * 1. Merge Sort: Stable O(N log N) sorting for Leaderboard and Transaction records.
 * 2. Quick Sort: Fast in-place O(N log N) sorting for Top Gainers and Losers.
 */
class Sorting {
public:
    /**
     * @brief Custom Merge Sort (Stable O(N log N))
     */
    template <typename T, typename Comparator = std::less<T>>
    static void mergeSort(std::vector<T>& arr, Comparator comp = Comparator()) {
        if (arr.size() <= 1) return;
        std::vector<T> temp(arr.size());
        mergeSortInternal(arr, temp, 0, arr.size() - 1, comp);
    }

    /**
     * @brief Custom Quick Sort (Average O(N log N))
     */
    template <typename T, typename Comparator = std::less<T>>
    static void quickSort(std::vector<T>& arr, Comparator comp = Comparator()) {
        if (arr.size() <= 1) return;
        quickSortInternal(arr, 0, static_cast<int>(arr.size() - 1), comp);
    }

    /**
     * @brief Visualizer Trace for Merge Sort (integer arrays)
     */
    static std::vector<SortStepLog> traceMergeSort(std::vector<int> arr) {
        std::vector<SortStepLog> logs;
        if (arr.empty()) return logs;

        SortStepLog initLog;
        initLog.algorithm = "Merge Sort";
        initLog.arrayState = arr;
        initLog.description = "Initial unsorted array";
        logs.push_back(initLog);

        std::vector<int> temp(arr.size());
        traceMergeSortInternal(arr, temp, 0, arr.size() - 1, logs);

        SortStepLog finalLog;
        finalLog.algorithm = "Merge Sort";
        finalLog.arrayState = arr;
        finalLog.description = "Array fully sorted";
        logs.push_back(finalLog);

        return logs;
    }

    /**
     * @brief Visualizer Trace for Quick Sort (integer arrays)
     */
    static std::vector<SortStepLog> traceQuickSort(std::vector<int> arr) {
        std::vector<SortStepLog> logs;
        if (arr.empty()) return logs;

        SortStepLog initLog;
        initLog.algorithm = "Quick Sort";
        initLog.arrayState = arr;
        initLog.description = "Initial unsorted array";
        logs.push_back(initLog);

        traceQuickSortInternal(arr, 0, static_cast<int>(arr.size() - 1), logs);

        SortStepLog finalLog;
        finalLog.algorithm = "Quick Sort";
        finalLog.arrayState = arr;
        finalLog.description = "Array fully sorted";
        logs.push_back(finalLog);

        return logs;
    }

private:
    template <typename T, typename Comparator>
    static void mergeSortInternal(std::vector<T>& arr, std::vector<T>& temp, size_t left, size_t right, Comparator comp) {
        if (left >= right) return;
        size_t mid = left + (right - left) / 2;
        mergeSortInternal(arr, temp, left, mid, comp);
        mergeSortInternal(arr, temp, mid + 1, right, comp);
        merge(arr, temp, left, mid, right, comp);
    }

    template <typename T, typename Comparator>
    static void merge(std::vector<T>& arr, std::vector<T>& temp, size_t left, size_t mid, size_t right, Comparator comp) {
        size_t i = left;
        size_t j = mid + 1;
        size_t k = left;

        while (i <= mid && j <= right) {
            if (comp(arr[j], arr[i])) { // arr[j] < arr[i]
                temp[k++] = std::move(arr[j++]);
            } else {
                temp[k++] = std::move(arr[i++]);
            }
        }

        while (i <= mid) {
            temp[k++] = std::move(arr[i++]);
        }
        while (j <= right) {
            temp[k++] = std::move(arr[j++]);
        }

        for (size_t idx = left; idx <= right; ++idx) {
            arr[idx] = std::move(temp[idx]);
        }
    }

    template <typename T, typename Comparator>
    static void quickSortInternal(std::vector<T>& arr, int low, int high, Comparator comp) {
        if (low < high) {
            int pivotIndex = partition(arr, low, high, comp);
            quickSortInternal(arr, low, pivotIndex - 1, comp);
            quickSortInternal(arr, pivotIndex + 1, high, comp);
        }
    }

    template <typename T, typename Comparator>
    static int partition(std::vector<T>& arr, int low, int high, Comparator comp) {
        T pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; ++j) {
            if (comp(arr[j], pivot)) {
                ++i;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    static void traceMergeSortInternal(std::vector<int>& arr, std::vector<int>& temp, size_t left, size_t right, std::vector<SortStepLog>& logs) {
        if (left >= right) return;
        size_t mid = left + (right - left) / 2;
        traceMergeSortInternal(arr, temp, left, mid, logs);
        traceMergeSortInternal(arr, temp, mid + 1, right, logs);

        size_t i = left;
        size_t j = mid + 1;
        size_t k = left;

        while (i <= mid && j <= right) {
            if (arr[j] < arr[i]) {
                temp[k++] = arr[j++];
            } else {
                temp[k++] = arr[i++];
            }
        }
        while (i <= mid) temp[k++] = arr[i++];
        while (j <= right) temp[k++] = arr[j++];

        for (size_t idx = left; idx <= right; ++idx) {
            arr[idx] = temp[idx];
        }

        SortStepLog log;
        log.algorithm = "Merge Sort";
        log.arrayState = arr;
        for (size_t idx = left; idx <= right; ++idx) {
            log.highlightedIndices.push_back(idx);
        }
        log.description = "Merged subarrays [" + std::to_string(left) + ".." + std::to_string(mid) + "] and [" + std::to_string(mid + 1) + ".." + std::to_string(right) + "]";
        logs.push_back(log);
    }

    static void traceQuickSortInternal(std::vector<int>& arr, int low, int high, std::vector<SortStepLog>& logs) {
        if (low < high) {
            int pivot = arr[high];
            int i = low - 1;

            for (int j = low; j < high; ++j) {
                if (arr[j] < pivot) {
                    ++i;
                    std::swap(arr[i], arr[j]);
                }
            }
            std::swap(arr[i + 1], arr[high]);
            int pivotIndex = i + 1;

            SortStepLog log;
            log.algorithm = "Quick Sort";
            log.arrayState = arr;
            log.highlightedIndices = {static_cast<size_t>(pivotIndex)};
            log.description = "Partitioned around pivot " + std::to_string(pivot) + " at index " + std::to_string(pivotIndex);
            logs.push_back(log);

            traceQuickSortInternal(arr, low, pivotIndex - 1, logs);
            traceQuickSortInternal(arr, pivotIndex + 1, high, logs);
        }
    }
};

} // namespace dsa
} // namespace tradeverse

#endif // TRADEVERSE_SORTING_HPP
