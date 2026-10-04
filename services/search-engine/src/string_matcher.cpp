#include "algosearch/string_matcher.hpp"
#include "algosearch/types.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace algosearch {

// ── KMP Algorithm ──────────────────────────────────────────────────

std::vector<int> StringMatcher::build_kmp_table(const std::string& pattern) {
    int m = static_cast<int>(pattern.size());
    std::vector<int> table(m, 0);
    int len = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            table[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = table[len - 1];
            } else {
                table[i] = 0;
                i++;
            }
        }
    }

    return table;
}

std::vector<int> StringMatcher::kmp_search(
    const std::string& text,
    const std::string& pattern
) {
    std::vector<int> result;
    if (pattern.empty() || text.empty()) return result;
    if (pattern.size() > text.size()) return result;

    // Normalize both to lowercase
    std::string lower_text = to_lower(text);
    std::string lower_pattern = to_lower(pattern);

    int n = static_cast<int>(lower_text.size());
    int m = static_cast<int>(lower_pattern.size());

    std::vector<int> table = build_kmp_table(lower_pattern);

    int i = 0; // text index
    int j = 0; // pattern index

    while (i < n) {
        if (lower_pattern[j] == lower_text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            result.push_back(i - j);
            j = table[j - 1];
        } else if (i < n && lower_pattern[j] != lower_text[i]) {
            if (j != 0) {
                j = table[j - 1];
            } else {
                i++;
            }
        }
    }

    return result;
}

bool StringMatcher::kmp_contains(const std::string& text, const std::string& pattern) {
    return !kmp_search(text, pattern).empty();
}

// ── Rabin-Karp Algorithm ───────────────────────────────────────────

long long StringMatcher::compute_hash(const std::string& s, int len, int prime) {
    long long hash = 0;
    const long long base = 256;
    const long long mod = 1000000007;

    for (int i = 0; i < len && i < static_cast<int>(s.size()); i++) {
        hash = (hash * base + static_cast<unsigned char>(s[i])) % mod;
    }
    (void)prime;
    return hash;
}

std::vector<int> StringMatcher::rabin_karp_search(
    const std::string& text,
    const std::string& pattern,
    int prime
) {
    std::vector<int> result;
    if (pattern.empty() || text.empty()) return result;
    if (pattern.size() > text.size()) return result;

    std::string lower_text = to_lower(text);
    std::string lower_pattern = to_lower(pattern);

    int n = static_cast<int>(lower_text.size());
    int m = static_cast<int>(lower_pattern.size());

    const long long base = 256;
    const long long mod = 1000000007;

    // Compute base^(m-1) % mod
    long long h = 1;
    for (int i = 0; i < m - 1; i++) {
        h = (h * base) % mod;
    }

    // Compute initial hashes
    long long pattern_hash = 0, text_hash = 0;
    for (int i = 0; i < m; i++) {
        pattern_hash = (pattern_hash * base + static_cast<unsigned char>(lower_pattern[i])) % mod;
        text_hash = (text_hash * base + static_cast<unsigned char>(lower_text[i])) % mod;
    }

    // Slide the pattern over text
    for (int i = 0; i <= n - m; i++) {
        if (pattern_hash == text_hash) {
            // Verify match (avoid false positives from hash collisions)
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (lower_text[i + j] != lower_pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                result.push_back(i);
            }
        }

        // Compute hash for next window
        if (i < n - m) {
            text_hash = (base * (text_hash - static_cast<unsigned char>(lower_text[i]) * h)
                        + static_cast<unsigned char>(lower_text[i + m])) % mod;
            if (text_hash < 0) text_hash += mod;
        }
    }

    (void)prime;
    return result;
}

std::vector<std::pair<std::string, int>> StringMatcher::rabin_karp_multi(
    const std::string& text,
    const std::vector<std::string>& patterns,
    int prime
) {
    std::vector<std::pair<std::string, int>> results;
    for (const auto& pattern : patterns) {
        auto matches = rabin_karp_search(text, pattern, prime);
        for (int pos : matches) {
            results.emplace_back(pattern, pos);
        }
    }
    return results;
}

// ── Levenshtein Distance ───────────────────────────────────────────

int StringMatcher::levenshtein_distance(
    const std::string& s1,
    const std::string& s2,
    int max_distance
) {
    int m = static_cast<int>(s1.size());
    int n = static_cast<int>(s2.size());

    // Quick rejection based on length difference
    if (std::abs(m - n) > max_distance) {
        return max_distance + 1;
    }

    // Use two-row DP to save memory: O(min(m,n)) space
    if (m < n) {
        return levenshtein_distance(s2, s1, max_distance);
    }

    std::vector<int> prev(n + 1);
    std::vector<int> curr(n + 1);

    // Initialize base case
    for (int j = 0; j <= n; j++) {
        prev[j] = j;
    }

    for (int i = 1; i <= m; i++) {
        curr[0] = i;
        int min_in_row = curr[0];

        for (int j = 1; j <= n; j++) {
            int cost = (std::tolower(static_cast<unsigned char>(s1[i-1])) ==
                       std::tolower(static_cast<unsigned char>(s2[j-1]))) ? 0 : 1;

            curr[j] = std::min({
                prev[j] + 1,       // deletion
                curr[j-1] + 1,     // insertion
                prev[j-1] + cost   // substitution
            });

            min_in_row = std::min(min_in_row, curr[j]);
        }

        // Early termination if all values in row exceed max_distance
        if (min_in_row > max_distance) {
            return max_distance + 1;
        }

        std::swap(prev, curr);
    }

    return prev[n];
}

bool StringMatcher::is_fuzzy_match(
    const std::string& s1,
    const std::string& s2,
    int threshold
) {
    return levenshtein_distance(s1, s2, threshold) <= threshold;
}

// ── Token Matching ─────────────────────────────────────────────────

double StringMatcher::token_match_score(
    const std::vector<std::string>& query_tokens,
    const std::string& target
) {
    if (query_tokens.empty() || target.empty()) return 0.0;

    std::string lower_target = to_lower(target);
    auto target_tokens = tokenize(target);

    int matches = 0;
    for (const auto& qt : query_tokens) {
        // Check exact token match or substring match
        for (const auto& tt : target_tokens) {
            if (tt == qt || tt.find(qt) != std::string::npos || qt.find(tt) != std::string::npos) {
                matches++;
                break;
            }
        }
    }

    return static_cast<double>(matches) / static_cast<double>(query_tokens.size());
}

double StringMatcher::fuzzy_token_match_score(
    const std::vector<std::string>& query_tokens,
    const std::string& target,
    int typo_threshold
) {
    if (query_tokens.empty() || target.empty()) return 0.0;

    auto target_tokens = tokenize(target);

    double total_score = 0.0;
    for (const auto& qt : query_tokens) {
        double best_match = 0.0;
        for (const auto& tt : target_tokens) {
            // Exact match
            if (tt == qt) {
                best_match = 1.0;
                break;
            }
            // Substring match
            if (tt.find(qt) != std::string::npos || qt.find(tt) != std::string::npos) {
                best_match = std::max(best_match, 0.8);
                continue;
            }
            // Fuzzy match
            int dist = levenshtein_distance(qt, tt, typo_threshold);
            if (dist <= typo_threshold) {
                double score = 1.0 - static_cast<double>(dist) / (typo_threshold + 1.0);
                best_match = std::max(best_match, score * 0.6);
            }
        }
        total_score += best_match;
    }

    return total_score / static_cast<double>(query_tokens.size());
}

} // namespace algosearch
