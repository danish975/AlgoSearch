#pragma once

#include <string>
#include <vector>

namespace algosearch {

/**
 * String matching algorithms for search functionality.
 * 
 * KMP:         O(n + m) exact pattern matching
 * Rabin-Karp:  O(n + m) average, O(nm) worst case with hash collisions
 * Levenshtein: O(n * m) edit distance for fuzzy matching
 * 
 * Usage guidelines:
 * - KMP: Best for exact substring matching in longer texts
 * - Rabin-Karp: Best for multi-pattern matching
 * - Levenshtein: Best for typo-tolerant search (bounded for performance)
 */
class StringMatcher {
public:
    // ── KMP Algorithm ──────────────────────────────────────────────

    // Find all occurrences of pattern in text using KMP
    // Returns list of starting indices
    static std::vector<int> kmp_search(const std::string& text, const std::string& pattern);

    // Check if pattern exists in text using KMP
    static bool kmp_contains(const std::string& text, const std::string& pattern);

    // ── Rabin-Karp Algorithm ───────────────────────────────────────

    // Find all occurrences of pattern in text using Rabin-Karp
    static std::vector<int> rabin_karp_search(
        const std::string& text,
        const std::string& pattern,
        int prime = 101
    );

    // Multi-pattern search: find any of the patterns in text
    static std::vector<std::pair<std::string, int>> rabin_karp_multi(
        const std::string& text,
        const std::vector<std::string>& patterns,
        int prime = 101
    );

    // ── Levenshtein Distance ───────────────────────────────────────

    // Compute edit distance between two strings
    // Bounded version: returns early if distance exceeds max_distance
    static int levenshtein_distance(
        const std::string& s1,
        const std::string& s2,
        int max_distance = 100
    );

    // Check if two strings are within edit distance threshold
    static bool is_fuzzy_match(
        const std::string& s1,
        const std::string& s2,
        int threshold = 2
    );

    // ── Token Matching ─────────────────────────────────────────────

    // Score how well query tokens match a target string
    // Returns 0.0 to 1.0 indicating match quality
    static double token_match_score(
        const std::vector<std::string>& query_tokens,
        const std::string& target
    );

    // Fuzzy token matching: allows typos in individual tokens
    static double fuzzy_token_match_score(
        const std::vector<std::string>& query_tokens,
        const std::string& target,
        int typo_threshold = 2
    );

private:
    // Build KMP failure function
    static std::vector<int> build_kmp_table(const std::string& pattern);

    // Compute rolling hash for Rabin-Karp
    static long long compute_hash(const std::string& s, int len, int prime);
};

} // namespace algosearch
