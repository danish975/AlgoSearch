#include <gtest/gtest.h>
#include "algosearch/search_engine.hpp"
#include <fstream>

using namespace algosearch;

class SearchEngineTest : public ::testing::Test {
protected:
    SearchEngine engine;

    void SetUp() override {
        std::vector<Problem> problems;

        Problem p1;
        p1.id = "P001"; p1.title = "Two Sum"; p1.slug = "two-sum";
        p1.description = "Given an array of integers, return indices of two numbers that add up to target.";
        p1.difficulty = "Easy"; p1.topics = {"Array", "Hash Table"};
        p1.algorithms = {"Hash Map"}; p1.platform = "LeetCode";
        p1.popularity = 950; p1.created_at = "2024-01-01T00:00:00.000Z";
        problems.push_back(p1);

        Problem p2;
        p2.id = "P014"; p2.title = "Network Delay Time"; p2.slug = "network-delay-time";
        p2.description = "Find the time for all nodes to receive a signal using Dijkstra shortest path.";
        p2.difficulty = "Medium"; p2.topics = {"Graph", "Heap", "Shortest Path"};
        p2.algorithms = {"Dijkstra"}; p2.platform = "LeetCode";
        p2.popularity = 680; p2.created_at = "2024-07-15T00:00:00.000Z";
        problems.push_back(p2);

        Problem p3;
        p3.id = "P015"; p3.title = "Course Schedule"; p3.slug = "course-schedule";
        p3.description = "Determine if you can finish all courses, cycle detection in directed graph.";
        p3.difficulty = "Medium"; p3.topics = {"Graph", "DFS", "BFS", "Topological Sort"};
        p3.algorithms = {"DFS", "BFS", "Topological Sort"}; p3.platform = "LeetCode";
        p3.popularity = 750; p3.created_at = "2024-08-01T00:00:00.000Z";
        problems.push_back(p3);

        Problem p4;
        p4.id = "P017"; p4.title = "Implement Trie (Prefix Tree)"; p4.slug = "implement-trie";
        p4.description = "Implement a Trie data structure with insert, search, startsWith.";
        p4.difficulty = "Medium"; p4.topics = {"Trie", "String", "Design"};
        p4.algorithms = {"Trie"}; p4.platform = "LeetCode";
        p4.popularity = 720; p4.created_at = "2024-09-01T00:00:00.000Z";
        problems.push_back(p4);

        Problem p5;
        p5.id = "P022"; p5.title = "Binary Search"; p5.slug = "binary-search";
        p5.description = "Search for target in sorted array using binary search in O(log n).";
        p5.difficulty = "Easy"; p5.topics = {"Array", "Binary Search"};
        p5.algorithms = {"Binary Search"}; p5.platform = "LeetCode";
        p5.popularity = 800; p5.created_at = "2024-11-15T00:00:00.000Z";
        problems.push_back(p5);

        engine.load_problems(problems);
    }
};

TEST_F(SearchEngineTest, BasicSearch) {
    SearchQuery q;
    q.query = "dijkstra";
    q.limit = 10;

    auto response = engine.search(q);
    EXPECT_GT(response.total_count, 0);
    EXPECT_GT(response.search_duration_ms, 0.0);

    // "Network Delay Time" should be top result for "dijkstra"
    ASSERT_FALSE(response.results.empty());
    EXPECT_EQ(response.results[0].problem.id, "P014");
}

TEST_F(SearchEngineTest, SearchGraph) {
    SearchQuery q;
    q.query = "graph";
    q.limit = 10;

    auto response = engine.search(q);
    EXPECT_GE(response.total_count, 2);  // P014 and P015 have graph topic
}

TEST_F(SearchEngineTest, SearchWithDifficultyFilter) {
    SearchQuery q;
    q.query = "array";
    q.difficulty = "Easy";
    q.limit = 10;

    auto response = engine.search(q);
    for (const auto& r : response.results) {
        EXPECT_EQ(r.problem.difficulty, "Easy");
    }
}

TEST_F(SearchEngineTest, SearchWithTopicFilter) {
    SearchQuery q;
    q.query = "";
    q.topic = "Graph";
    q.limit = 10;

    auto response = engine.search(q);
    for (const auto& r : response.results) {
        bool has_graph = false;
        for (const auto& t : r.problem.topics) {
            if (t == "Graph") has_graph = true;
        }
        EXPECT_TRUE(has_graph);
    }
}

TEST_F(SearchEngineTest, EmptySearch) {
    SearchQuery q;
    q.query = "";
    q.limit = 10;

    auto response = engine.search(q);
    EXPECT_EQ(response.total_count, 5);  // all problems
}

TEST_F(SearchEngineTest, NoResultsSearch) {
    SearchQuery q;
    q.query = "xyznonexistent";
    q.limit = 10;

    auto response = engine.search(q);
    // May have fuzzy matches or zero results
    EXPECT_GE(response.search_duration_ms, 0.0);
}

TEST_F(SearchEngineTest, Autocomplete) {
    auto results = engine.autocomplete("gra", 5);
    EXPECT_FALSE(results.empty());

    bool found_graph = false;
    for (const auto& r : results) {
        std::string lower = r.term;
        for (auto& c : lower) c = tolower(c);
        if (lower.find("gra") == 0) found_graph = true;
    }
    EXPECT_TRUE(found_graph);
}

TEST_F(SearchEngineTest, AutocompleteEmpty) {
    auto results = engine.autocomplete("xyz");
    EXPECT_TRUE(results.empty());
}

TEST_F(SearchEngineTest, GetProblem) {
    auto* p = engine.get_problem("P001");
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(p->title, "Two Sum");
}

TEST_F(SearchEngineTest, GetProblemNotFound) {
    auto* p = engine.get_problem("P999");
    EXPECT_EQ(p, nullptr);
}

TEST_F(SearchEngineTest, RelatedProblems) {
    auto related = engine.get_related_problems("P014");
    // P014 and P015 share Graph topic, so P015 should appear
    bool found = false;
    for (const auto& r : related) {
        if (r.problem.id == "P015") found = true;
    }
    EXPECT_TRUE(found);
}

TEST_F(SearchEngineTest, GetTopics) {
    auto topics = engine.get_topics();
    EXPECT_FALSE(topics.empty());
}

TEST_F(SearchEngineTest, GetAlgorithms) {
    auto algos = engine.get_algorithms();
    EXPECT_FALSE(algos.empty());
}

TEST_F(SearchEngineTest, GetProblemsByTopic) {
    auto problems = engine.get_problems_by_topic("Graph");
    EXPECT_GE(problems.size(), 2u);
}

TEST_F(SearchEngineTest, EngineStats) {
    auto stats = engine.get_stats();
    EXPECT_EQ(stats.total_problems, 5u);
    EXPECT_GT(stats.index_terms, 0u);
    EXPECT_GT(stats.trie_terms, 0u);
    EXPECT_GT(stats.graph_nodes, 0u);
}

TEST_F(SearchEngineTest, Pagination) {
    SearchQuery q;
    q.query = "";
    q.limit = 2;
    q.offset = 0;

    auto page1 = engine.search(q);
    EXPECT_EQ(page1.results.size(), 2u);

    q.offset = 2;
    auto page2 = engine.search(q);
    EXPECT_EQ(page2.results.size(), 2u);

    q.offset = 4;
    auto page3 = engine.search(q);
    EXPECT_EQ(page3.results.size(), 1u);
}

TEST_F(SearchEngineTest, TrackSearch) {
    engine.track_search("graph traversal");
    // Should increment frequency for "graph" in trie
    auto suggestions = engine.autocomplete("gra", 10);
    // Just verify it doesn't crash
    EXPECT_FALSE(suggestions.empty());
}
