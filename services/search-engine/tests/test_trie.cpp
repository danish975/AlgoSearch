#include <gtest/gtest.h>
#include "algosearch/trie.hpp"

using namespace algosearch;

class TrieTest : public ::testing::Test {
protected:
    Trie trie;
};

TEST_F(TrieTest, InsertAndSearch) {
    trie.insert("dijkstra");
    EXPECT_TRUE(trie.search("dijkstra"));
    EXPECT_TRUE(trie.search("Dijkstra"));  // case insensitive
    EXPECT_FALSE(trie.search("dijkstr"));
    EXPECT_FALSE(trie.search("dijkstraa"));
}

TEST_F(TrieTest, EmptyInput) {
    trie.insert("");
    EXPECT_EQ(trie.size(), 0u);
    EXPECT_FALSE(trie.search(""));
    auto suggestions = trie.autocomplete("");
    EXPECT_TRUE(suggestions.empty());
}

TEST_F(TrieTest, DuplicateInsertion) {
    trie.insert("graph");
    trie.insert("graph");
    trie.insert("Graph");  // same after normalization
    EXPECT_EQ(trie.size(), 1u);
    EXPECT_TRUE(trie.search("graph"));
}

TEST_F(TrieTest, StartsWith) {
    trie.insert("graph");
    trie.insert("greedy");
    trie.insert("dynamic");
    EXPECT_TRUE(trie.starts_with("gra"));
    EXPECT_TRUE(trie.starts_with("gre"));
    EXPECT_TRUE(trie.starts_with("dyn"));
    EXPECT_FALSE(trie.starts_with("xyz"));
}

TEST_F(TrieTest, Remove) {
    trie.insert("graph");
    trie.insert("greedy");
    EXPECT_EQ(trie.size(), 2u);

    EXPECT_TRUE(trie.remove("graph"));
    EXPECT_FALSE(trie.search("graph"));
    EXPECT_EQ(trie.size(), 1u);

    EXPECT_FALSE(trie.remove("nonexistent"));
    EXPECT_EQ(trie.size(), 1u);
}

TEST_F(TrieTest, Autocomplete) {
    trie.insert("graph", "topic");
    trie.insert("greedy", "topic");
    trie.insert("group anagrams", "problem");
    trie.insert("dynamic", "topic");

    auto results = trie.autocomplete("gr", 10);
    EXPECT_GE(results.size(), 3u);

    // All results should start with "gr" (case-insensitive)
    for (const auto& r : results) {
        std::string lower = r.term;
        for (auto& c : lower) c = tolower(c);
        EXPECT_TRUE(lower.substr(0, 2) == "gr");
    }
}

TEST_F(TrieTest, AutocompleteLimit) {
    for (int i = 0; i < 20; i++) {
        trie.insert("term" + std::to_string(i));
    }

    auto results = trie.autocomplete("term", 5);
    EXPECT_LE(results.size(), 5u);
}

TEST_F(TrieTest, AutocompleteMissingPrefix) {
    trie.insert("graph");
    auto results = trie.autocomplete("xyz");
    EXPECT_TRUE(results.empty());
}

TEST_F(TrieTest, FrequencyIncrement) {
    trie.insert("graph");
    trie.increment_frequency("graph");
    trie.increment_frequency("graph");

    auto results = trie.autocomplete("gra", 10);
    ASSERT_FALSE(results.empty());
    EXPECT_EQ(results[0].frequency, 3);  // 1 initial + 2 increments
}

TEST_F(TrieTest, CaseNormalization) {
    trie.insert("Dijkstra");
    EXPECT_TRUE(trie.search("dijkstra"));
    EXPECT_TRUE(trie.search("DIJKSTRA"));
    EXPECT_TRUE(trie.search("Dijkstra"));
}

TEST_F(TrieTest, LargeDictionary) {
    for (int i = 0; i < 1000; i++) {
        trie.insert("word" + std::to_string(i));
    }
    EXPECT_EQ(trie.size(), 1000u);
    EXPECT_TRUE(trie.search("word500"));
    EXPECT_TRUE(trie.search("word999"));
}

TEST_F(TrieTest, Clear) {
    trie.insert("a");
    trie.insert("b");
    trie.clear();
    EXPECT_EQ(trie.size(), 0u);
    EXPECT_FALSE(trie.search("a"));
}
