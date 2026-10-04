#include <gtest/gtest.h>
#include "algosearch/inverted_index.hpp"
#include "algosearch/types.hpp"

using namespace algosearch;

class InvertedIndexTest : public ::testing::Test {
protected:
    InvertedIndex index;

    void SetUp() override {
        // Index some documents
        index.add_document("P001", "title", {"two", "sum"}, 2.0);
        index.add_document("P001", "description", {"array", "integers", "target", "indices", "numbers", "add"}, 1.0);

        index.add_document("P002", "title", {"add", "two", "numbers"}, 2.0);
        index.add_document("P002", "description", {"linked", "list", "non", "negative", "integers", "digits", "reverse"}, 1.0);

        index.add_document("P003", "title", {"longest", "substring", "without", "repeating"}, 2.0);
        index.add_document("P003", "description", {"string", "length", "longest", "substring", "sliding", "window"}, 1.0);
    }
};

TEST_F(InvertedIndexTest, SearchTerm) {
    auto results = index.search_term("two");
    EXPECT_EQ(results.size(), 2u);  // P001 and P002
}

TEST_F(InvertedIndexTest, SearchTermNotFound) {
    auto results = index.search_term("nonexistent");
    EXPECT_TRUE(results.empty());
}

TEST_F(InvertedIndexTest, BooleanAnd) {
    auto results = index.search_and({"two", "add"});
    EXPECT_FALSE(results.empty());
    // Both P001 and P002 contain "two" and "add"
}

TEST_F(InvertedIndexTest, BooleanAndEmpty) {
    auto results = index.search_and({"two", "xyz"});
    EXPECT_TRUE(results.empty());
}

TEST_F(InvertedIndexTest, BooleanOr) {
    auto results = index.search_or({"linked", "string"});
    EXPECT_EQ(results.size(), 2u);  // P002 and P003
}

TEST_F(InvertedIndexTest, TfIdf) {
    double score = index.tf_idf("two", "P001");
    EXPECT_GT(score, 0.0);

    double score_missing = index.tf_idf("two", "P003");
    EXPECT_EQ(score_missing, 0.0);
}

TEST_F(InvertedIndexTest, ScoredSearch) {
    auto scores = index.search_scored({"longest", "substring"});
    EXPECT_TRUE(scores.count("P003") > 0);
    EXPECT_GT(scores["P003"], 0.0);
}

TEST_F(InvertedIndexTest, DocumentCount) {
    EXPECT_EQ(index.document_count(), 3u);
}

TEST_F(InvertedIndexTest, DocumentFrequency) {
    EXPECT_EQ(index.document_frequency("two"), 2);
    EXPECT_EQ(index.document_frequency("longest"), 1);
    EXPECT_EQ(index.document_frequency("xyz"), 0);
}

TEST_F(InvertedIndexTest, RemoveDocument) {
    index.remove_document("P001");
    EXPECT_EQ(index.document_count(), 2u);
    auto results = index.search_term("sum");
    EXPECT_TRUE(results.empty());  // "sum" was only in P001
}

TEST_F(InvertedIndexTest, FieldWeighting) {
    // Title should get higher weight
    double title_score = index.tf_idf("two", "P001");
    // "two" appears in title of P001 (weight 2.0) and in title of P002 (weight 2.0)
    EXPECT_GT(title_score, 0.0);
}

TEST_F(InvertedIndexTest, Clear) {
    index.clear();
    EXPECT_EQ(index.document_count(), 0u);
    EXPECT_EQ(index.term_count(), 0u);
}

TEST_F(InvertedIndexTest, ContainsTerm) {
    EXPECT_TRUE(index.contains_term("two"));
    EXPECT_FALSE(index.contains_term("xyz"));
}
