#include "algosearch/min_heap.hpp"
#include <algorithm>

namespace algosearch {

TopKRetriever::TopKRetriever(int k) : k_(k) {}

void TopKRetriever::push(const ScoredResult& result) {
    if (k_ <= 0) return;

    if (static_cast<int>(heap_.size()) < k_) {
        heap_.push(result);
    } else if (result > heap_.top()) {
        // New result is better than the current worst in top-K
        heap_.pop();
        heap_.push(result);
    }
    // If result <= heap_.top(), discard it (not in top-K)
}

void TopKRetriever::push_batch(const std::vector<ScoredResult>& results) {
    for (const auto& r : results) {
        push(r);
    }
}

std::vector<ScoredResult> TopKRetriever::get_results() {
    std::vector<ScoredResult> results;
    results.reserve(heap_.size());

    while (!heap_.empty()) {
        results.push_back(heap_.top());
        heap_.pop();
    }

    // Heap gives us ascending order; reverse for descending (best first)
    std::reverse(results.begin(), results.end());
    return results;
}

size_t TopKRetriever::size() const {
    return heap_.size();
}

void TopKRetriever::reset(int k) {
    k_ = k;
    // Clear the heap
    while (!heap_.empty()) {
        heap_.pop();
    }
}

double TopKRetriever::min_score() const {
    if (heap_.empty()) return 0.0;
    return heap_.top().score;
}

bool TopKRetriever::is_full() const {
    return static_cast<int>(heap_.size()) >= k_;
}

} // namespace algosearch
