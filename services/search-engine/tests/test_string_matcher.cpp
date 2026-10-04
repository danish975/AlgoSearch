#include <gtest/gtest.h>
#include "algosearch/string_matcher.hpp"

using namespace algosearch;

// ── KMP Tests ──────────────────────────────────────────────────────

TEST(KMPTest, BasicMatch) {
    auto results = StringMatcher::kmp_search("hello world", "world");
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0], 6);
}

TEST(KMPTest, MultipleMatches) {
    auto results = StringMatcher::kmp_search("abababab", "ab");
    EXPECT_EQ(results.size(), 4u);
}

TEST(KMPTest, NoMatch) {
    auto results = StringMatcher::kmp_search("hello", "xyz");
    EXPECT_TRUE(results.empty());
}

TEST(KMPTest, EmptyPattern) {
    auto results = StringMatcher::kmp_search("hello", "");
    EXPECT_TRUE(results.empty());
}

TEST(KMPTest, EmptyText) {
    auto results = StringMatcher::kmp_search("", "hello");
    EXPECT_TRUE(results.empty());
}

TEST(KMPTest, PatternLongerThanText) {
    auto results = StringMatcher::kmp_search("hi", "hello");
    EXPECT_TRUE(results.empty());
}

TEST(KMPTest, CaseInsensitive) {
    EXPECT_TRUE(StringMatcher::kmp_contains("Dijkstra Algorithm", "dijkstra"));
    EXPECT_TRUE(StringMatcher::kmp_contains("GRAPH traversal", "graph"));
}

TEST(KMPTest, Contains) {
    EXPECT_TRUE(StringMatcher::kmp_contains("binary search tree", "search"));
    EXPECT_FALSE(StringMatcher::kmp_contains("binary search tree", "sort"));
}

// ── Rabin-Karp Tests ───────────────────────────────────────────────

TEST(RabinKarpTest, BasicMatch) {
    auto results = StringMatcher::rabin_karp_search("hello world", "world");
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0], 6);
}

TEST(RabinKarpTest, MultipleMatches) {
    auto results = StringMatcher::rabin_karp_search("aaaa", "aa");
    EXPECT_EQ(results.size(), 3u);
}

TEST(RabinKarpTest, NoMatch) {
    auto results = StringMatcher::rabin_karp_search("hello", "xyz");
    EXPECT_TRUE(results.empty());
}

TEST(RabinKarpTest, MultiPatternSearch) {
    auto results = StringMatcher::rabin_karp_multi(
        "binary search tree traversal",
        {"binary", "tree", "sort"}
    );
    // "binary" and "tree" should be found, "sort" should not
    EXPECT_EQ(results.size(), 2u);
}

// ── Levenshtein Tests ──────────────────────────────────────────────

TEST(LevenshteinTest, IdenticalStrings) {
    EXPECT_EQ(StringMatcher::levenshtein_distance("hello", "hello"), 0);
}

TEST(LevenshteinTest, SingleInsertion) {
    EXPECT_EQ(StringMatcher::levenshtein_distance("cat", "cats"), 1);
}

TEST(LevenshteinTest, SingleDeletion) {
    EXPECT_EQ(StringMatcher::levenshtein_distance("cats", "cat"), 1);
}

TEST(LevenshteinTest, SingleSubstitution) {
    EXPECT_EQ(StringMatcher::levenshtein_distance("cat", "car"), 1);
}

TEST(LevenshteinTest, CompletelyDifferent) {
    EXPECT_EQ(StringMatcher::levenshtein_distance("abc", "xyz"), 3);
}

TEST(LevenshteinTest, EmptyStrings) {
    EXPECT_EQ(StringMatcher::levenshtein_distance("", ""), 0);
    EXPECT_EQ(StringMatcher::levenshtein_distance("hello", ""), 5);
    EXPECT_EQ(StringMatcher::levenshtein_distance("", "hello"), 5);
}

TEST(LevenshteinTest, CaseInsensitive) {
    EXPECT_EQ(StringMatcher::levenshtein_distance("Hello", "hello"), 0);
}

TEST(LevenshteinTest, BoundedEarlyTermination) {
    int dist = StringMatcher::levenshtein_distance("abc", "xyz", 1);
    EXPECT_GT(dist, 1);  // should exceed max_distance
}

TEST(LevenshteinTest, FuzzyMatch) {
    EXPECT_TRUE(StringMatcher::is_fuzzy_match("graph", "grahp", 2));
    EXPECT_TRUE(StringMatcher::is_fuzzy_match("dijkstra", "dijkstra", 0));
    EXPECT_FALSE(StringMatcher::is_fuzzy_match("abc", "xyz", 2));
}

// ── Token Matching Tests ───────────────────────────────────────────

TEST(TokenMatchTest, ExactTokenMatch) {
    double score = StringMatcher::token_match_score(
        {"binary", "search"}, "Binary Search Tree"
    );
    EXPECT_GT(score, 0.5);
}

TEST(TokenMatchTest, NoMatch) {
    double score = StringMatcher::token_match_score(
        {"xyz", "abc"}, "Binary Search Tree"
    );
    EXPECT_EQ(score, 0.0);
}

TEST(TokenMatchTest, PartialMatch) {
    double score = StringMatcher::token_match_score(
        {"binary", "sort"}, "Binary Search Tree"
    );
    EXPECT_GT(score, 0.0);
    EXPECT_LT(score, 1.0);
}

TEST(TokenMatchTest, EmptyInput) {
    EXPECT_EQ(StringMatcher::token_match_score({}, "hello"), 0.0);
    EXPECT_EQ(StringMatcher::token_match_score({"hello"}, ""), 0.0);
}

TEST(TokenMatchTest, FuzzyTokenMatch) {
    double score = StringMatcher::fuzzy_token_match_score(
        {"grahp", "traversl"}, "Graph Traversal", 2
    );
    EXPECT_GT(score, 0.0);
}
