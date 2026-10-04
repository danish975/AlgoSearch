#include <gtest/gtest.h>
#include "algosearch/ranking_engine.hpp"

using namespace algosearch;

class RankingTest : public ::testing::Test {
protected:
    RankingEngine ranker;

    Problem make_problem(
        const std::string& id,
        const std::string& title,
        const std::string& difficulty,
        const std::vector<std::string>& topics,
        double popularity = 500.0
    ) {
        Problem p;
        p.id = id;
        p.title = title;
        p.difficulty = difficulty;
        p.topics = topics;
        p.popularity = popularity;
        p.created_at = "2025-01-01T00:00:00.000Z";
        return p;
    }
};

TEST_F(RankingTest, TitleMatchBoost) {
    auto p1 = make_problem("P1", "Dijkstra Shortest Path", "Medium", {"Graph"});
    auto p2 = make_problem("P2", "Number of Islands", "Medium", {"Graph"});

    auto r1 = ranker.score(p1, {"dijkstra"});
    auto r2 = ranker.score(p2, {"dijkstra"});

    EXPECT_GT(r1.score, r2.score);
}

TEST_F(RankingTest, DifficultyFilter) {
    auto p = make_problem("P1", "Two Sum", "Easy", {"Array"});

    auto easy = ranker.score(p, {"sum"}, "Easy");
    auto hard = ranker.score(p, {"sum"}, "Hard");

    EXPECT_GT(easy.score, hard.score);
}

TEST_F(RankingTest, PopularityInfluence) {
    auto popular = make_problem("P1", "Two Sum", "Easy", {"Array"}, 900.0);
    auto unpopular = make_problem("P2", "Two Sum", "Easy", {"Array"}, 100.0);

    auto r1 = ranker.score(popular, {"sum"});
    auto r2 = ranker.score(unpopular, {"sum"});

    EXPECT_GT(r1.score, r2.score);
}

TEST_F(RankingTest, ScoreBreakdown) {
    auto p = make_problem("P1", "Binary Search", "Easy", {"Array"});
    auto result = ranker.score(p, {"binary", "search"});

    EXPECT_TRUE(result.score_breakdown.count("title_match") > 0);
    EXPECT_TRUE(result.score_breakdown.count("keyword_relevance") > 0);
    EXPECT_TRUE(result.score_breakdown.count("topic_similarity") > 0);
    EXPECT_TRUE(result.score_breakdown.count("difficulty_match") > 0);
    EXPECT_TRUE(result.score_breakdown.count("popularity") > 0);
    EXPECT_TRUE(result.score_breakdown.count("recency") > 0);
}

TEST_F(RankingTest, DeterministicTieBreaking) {
    auto p1 = make_problem("P001", "Test A", "Easy", {});
    auto p2 = make_problem("P002", "Test B", "Easy", {});

    std::vector<std::pair<Problem, double>> candidates = {{p1, 0.0}, {p2, 0.0}};
    auto ranked = ranker.rank(candidates, {});

    ASSERT_EQ(ranked.size(), 2u);
    // Should be deterministic across runs
    EXPECT_EQ(ranked[0].problem.id, "P001");
}

TEST_F(RankingTest, RankBatch) {
    std::vector<std::pair<Problem, double>> candidates;
    candidates.emplace_back(make_problem("P1", "Binary Tree", "Easy", {"Tree"}), 1.0);
    candidates.emplace_back(make_problem("P2", "Binary Search", "Easy", {"Array"}), 2.0);
    candidates.emplace_back(make_problem("P3", "Binary Search Tree", "Medium", {"Tree"}), 1.5);

    auto ranked = ranker.rank(candidates, {"binary", "search"});

    EXPECT_EQ(ranked.size(), 3u);
    // Scores should be descending
    for (size_t i = 1; i < ranked.size(); i++) {
        EXPECT_LE(ranked[i].score, ranked[i-1].score);
    }
}

TEST_F(RankingTest, EmptyQuery) {
    auto p = make_problem("P1", "Two Sum", "Easy", {"Array"});
    auto result = ranker.score(p, {});
    // Should still produce a valid score (from popularity, recency, etc.)
    EXPECT_GE(result.score, 0.0);
}

TEST_F(RankingTest, CustomWeights) {
    RankingEngine::Weights w;
    w.title_match = 1.0;
    w.keyword_relevance = 0.0;
    w.topic_similarity = 0.0;
    w.difficulty_match = 0.0;
    w.popularity = 0.0;
    w.recency = 0.0;

    RankingEngine custom_ranker(w);

    auto p = make_problem("P1", "Dijkstra", "Easy", {"Graph"});
    auto result = custom_ranker.score(p, {"dijkstra"});

    // With only title match weight, score should equal title_match subscore
    EXPECT_DOUBLE_EQ(result.score, result.score_breakdown["title_match"]);
}

TEST_F(RankingTest, TopicSimilarity) {
    auto p = make_problem("P1", "Problem X", "Easy", {"Graph", "DFS"});
    auto result = ranker.score(p, {"graph"});

    EXPECT_GT(result.score_breakdown["topic_similarity"], 0.0);
}
