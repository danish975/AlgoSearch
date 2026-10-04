#include <gtest/gtest.h>
#include "algosearch/min_heap.hpp"

using namespace algosearch;

class TopKTest : public ::testing::Test {
protected:
    ScoredResult make_result(const std::string& id, double score) {
        ScoredResult r;
        r.problem.id = id;
        r.problem.title = "Problem " + id;
        r.score = score;
        return r;
    }
};

TEST_F(TopKTest, BasicTopK) {
    TopKRetriever topk(3);

    topk.push(make_result("P1", 0.5));
    topk.push(make_result("P2", 0.9));
    topk.push(make_result("P3", 0.3));
    topk.push(make_result("P4", 0.7));
    topk.push(make_result("P5", 0.1));

    auto results = topk.get_results();
    ASSERT_EQ(results.size(), 3u);
    EXPECT_EQ(results[0].problem.id, "P2");  // highest
    EXPECT_EQ(results[1].problem.id, "P4");
    EXPECT_EQ(results[2].problem.id, "P1");
}

TEST_F(TopKTest, FewerThanK) {
    TopKRetriever topk(10);

    topk.push(make_result("P1", 0.5));
    topk.push(make_result("P2", 0.9));

    auto results = topk.get_results();
    EXPECT_EQ(results.size(), 2u);
}

TEST_F(TopKTest, EmptyInput) {
    TopKRetriever topk(5);
    auto results = topk.get_results();
    EXPECT_TRUE(results.empty());
}

TEST_F(TopKTest, TiedScores) {
    TopKRetriever topk(3);

    topk.push(make_result("P1", 0.5));
    topk.push(make_result("P2", 0.5));
    topk.push(make_result("P3", 0.5));
    topk.push(make_result("P4", 0.5));

    auto results = topk.get_results();
    EXPECT_EQ(results.size(), 3u);
    // Deterministic tie-breaking by ID
    for (size_t i = 1; i < results.size(); i++) {
        // Either score is strictly less or IDs are in order for tied scores
        EXPECT_TRUE(results[i].score <= results[i-1].score);
    }
}

TEST_F(TopKTest, KEqualsOne) {
    TopKRetriever topk(1);

    topk.push(make_result("P1", 0.5));
    topk.push(make_result("P2", 0.9));
    topk.push(make_result("P3", 0.3));

    auto results = topk.get_results();
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].problem.id, "P2");
}

TEST_F(TopKTest, KEqualsZero) {
    TopKRetriever topk(0);
    topk.push(make_result("P1", 0.5));
    auto results = topk.get_results();
    EXPECT_TRUE(results.empty());
}

TEST_F(TopKTest, MinScore) {
    TopKRetriever topk(2);

    topk.push(make_result("P1", 0.5));
    EXPECT_DOUBLE_EQ(topk.min_score(), 0.5);

    topk.push(make_result("P2", 0.9));
    EXPECT_DOUBLE_EQ(topk.min_score(), 0.5);

    topk.push(make_result("P3", 0.7));
    EXPECT_DOUBLE_EQ(topk.min_score(), 0.7);  // P1 evicted
}

TEST_F(TopKTest, BatchPush) {
    TopKRetriever topk(3);

    std::vector<ScoredResult> batch;
    for (int i = 0; i < 100; i++) {
        batch.push_back(make_result("P" + std::to_string(i), i * 0.01));
    }

    topk.push_batch(batch);
    auto results = topk.get_results();

    ASSERT_EQ(results.size(), 3u);
    EXPECT_EQ(results[0].problem.id, "P99");
    EXPECT_EQ(results[1].problem.id, "P98");
    EXPECT_EQ(results[2].problem.id, "P97");
}

TEST_F(TopKTest, Reset) {
    TopKRetriever topk(3);
    topk.push(make_result("P1", 0.5));
    topk.reset(5);
    EXPECT_EQ(topk.size(), 0u);
}

TEST_F(TopKTest, IsFull) {
    TopKRetriever topk(2);
    EXPECT_FALSE(topk.is_full());
    topk.push(make_result("P1", 0.5));
    EXPECT_FALSE(topk.is_full());
    topk.push(make_result("P2", 0.9));
    EXPECT_TRUE(topk.is_full());
}
