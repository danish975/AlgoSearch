#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace algosearch {

/**
 * Trie data structure for prefix-based autocomplete suggestions.
 * 
 * Supports insert, remove, prefix search, and ranked suggestion retrieval.
 * All inputs are normalized to lowercase before operations.
 * 
 * Complexity:
 *   Insert:  O(L) where L = term length
 *   Search:  O(L + S) where S = subtree size visited
 *   Remove:  O(L)
 */
class Trie {
public:
    Trie();
    ~Trie();

    // Insert a term with an optional category (e.g., "problem", "topic", "algorithm")
    void insert(const std::string& term, const std::string& category = "");

    // Remove a term. Returns true if the term existed and was removed.
    bool remove(const std::string& term);

    // Check if a term exists exactly
    bool search(const std::string& term) const;

    // Check if any term starts with the given prefix
    bool starts_with(const std::string& prefix) const;

    // Get ranked autocomplete suggestions for a prefix
    // Returns up to `limit` suggestions, sorted by frequency (descending)
    struct Suggestion {
        std::string term;
        int frequency;
        std::string category;
    };

    std::vector<Suggestion> autocomplete(const std::string& prefix, int limit = 8) const;

    // Increment frequency counter for a term (for search popularity tracking)
    void increment_frequency(const std::string& term);

    // Get total number of terms stored
    size_t size() const;

    // Clear all terms
    void clear();

private:
    struct TrieNode {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        bool is_end = false;
        int frequency = 1;
        std::string category;
        std::string full_term;  // stored at leaf for efficient retrieval
    };

    std::unique_ptr<TrieNode> root_;
    size_t size_;

    // Normalize input to lowercase
    std::string normalize(const std::string& s) const;

    // DFS to collect all terms under a node
    void collect_suggestions(
        const TrieNode* node,
        std::vector<Suggestion>& results,
        int limit
    ) const;

    // Find the node at end of prefix path
    const TrieNode* find_node(const std::string& prefix) const;
    TrieNode* find_node_mut(const std::string& prefix);
};

} // namespace algosearch
