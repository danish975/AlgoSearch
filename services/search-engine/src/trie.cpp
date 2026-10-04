#include "algosearch/trie.hpp"
#include "algosearch/types.hpp"
#include <algorithm>
#include <queue>

namespace algosearch {

Trie::Trie() : root_(std::make_unique<TrieNode>()), size_(0) {}
Trie::~Trie() = default;

std::string Trie::normalize(const std::string& s) const {
    return to_lower(s);
}

void Trie::insert(const std::string& term, const std::string& category) {
    if (term.empty()) return;

    std::string normalized = normalize(term);
    TrieNode* node = root_.get();

    for (char c : normalized) {
        if (!node->children.count(c)) {
            node->children[c] = std::make_unique<TrieNode>();
        }
        node = node->children[c].get();
    }

    if (!node->is_end) {
        size_++;
    }

    node->is_end = true;
    node->full_term = term;  // preserve original casing
    node->category = category;
    // Don't reset frequency on duplicate insert
}

bool Trie::remove(const std::string& term) {
    if (term.empty()) return false;

    std::string normalized = normalize(term);
    TrieNode* node = root_.get();

    // Navigate to the node
    for (char c : normalized) {
        if (!node->children.count(c)) {
            return false;  // term not found
        }
        node = node->children[c].get();
    }

    if (!node->is_end) {
        return false;  // term not found
    }

    node->is_end = false;
    node->full_term.clear();
    size_--;
    return true;
}

bool Trie::search(const std::string& term) const {
    const TrieNode* node = find_node(normalize(term));
    return node != nullptr && node->is_end;
}

bool Trie::starts_with(const std::string& prefix) const {
    return find_node(normalize(prefix)) != nullptr;
}

std::vector<Trie::Suggestion> Trie::autocomplete(
    const std::string& prefix, int limit
) const {
    if (prefix.empty() || limit <= 0) return {};

    std::string normalized = normalize(prefix);
    const TrieNode* node = find_node(normalized);

    if (!node) return {};

    std::vector<Suggestion> results;
    collect_suggestions(node, results, limit * 4);  // collect more than needed for sorting

    // Sort by frequency descending, then alphabetically for stability
    std::sort(results.begin(), results.end(), [](const Suggestion& a, const Suggestion& b) {
        if (a.frequency != b.frequency) return a.frequency > b.frequency;
        return a.term < b.term;
    });

    if (static_cast<int>(results.size()) > limit) {
        results.resize(limit);
    }

    return results;
}

void Trie::increment_frequency(const std::string& term) {
    TrieNode* node = find_node_mut(normalize(term));
    if (node && node->is_end) {
        node->frequency++;
    }
}

size_t Trie::size() const {
    return size_;
}

void Trie::clear() {
    root_ = std::make_unique<TrieNode>();
    size_ = 0;
}

const Trie::TrieNode* Trie::find_node(const std::string& prefix) const {
    const TrieNode* node = root_.get();
    for (char c : prefix) {
        auto it = node->children.find(c);
        if (it == node->children.end()) {
            return nullptr;
        }
        node = it->second.get();
    }
    return node;
}

Trie::TrieNode* Trie::find_node_mut(const std::string& prefix) {
    TrieNode* node = root_.get();
    for (char c : prefix) {
        auto it = node->children.find(c);
        if (it == node->children.end()) {
            return nullptr;
        }
        node = it->second.get();
    }
    return node;
}

void Trie::collect_suggestions(
    const TrieNode* node,
    std::vector<Suggestion>& results,
    int limit
) const {
    if (!node || static_cast<int>(results.size()) >= limit) return;

    if (node->is_end) {
        results.push_back({
            node->full_term,
            node->frequency,
            node->category
        });
    }

    // BFS to collect suggestions level by level
    for (const auto& [ch, child] : node->children) {
        collect_suggestions(child.get(), results, limit);
        if (static_cast<int>(results.size()) >= limit) break;
    }
}

} // namespace algosearch
