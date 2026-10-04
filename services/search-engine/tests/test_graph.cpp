#include <gtest/gtest.h>
#include "algosearch/graph.hpp"

using namespace algosearch;

class GraphTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        graph.add_node("P001", "problem", "Two Sum");
        graph.add_node("P002", "problem", "Network Delay Time");
        graph.add_node("P003", "problem", "Course Schedule");
        graph.add_node("topic:array", "topic", "Array");
        graph.add_node("topic:graph", "topic", "Graph");
        graph.add_node("topic:heap", "topic", "Heap");
        graph.add_node("algo:dijkstra", "algorithm", "Dijkstra");
        graph.add_node("algo:bfs", "algorithm", "BFS");

        graph.add_bidirectional_edge("P001", "topic:array", 1.0);
        graph.add_bidirectional_edge("P002", "topic:graph", 1.0);
        graph.add_bidirectional_edge("P002", "topic:heap", 1.0);
        graph.add_bidirectional_edge("P002", "algo:dijkstra", 1.0);
        graph.add_bidirectional_edge("P003", "topic:graph", 1.0);
        graph.add_bidirectional_edge("P003", "algo:bfs", 1.0);
    }
};

TEST_F(GraphTest, NodeOperations) {
    EXPECT_TRUE(graph.has_node("P001"));
    EXPECT_FALSE(graph.has_node("P999"));
    EXPECT_EQ(graph.node_count(), 8u);
}

TEST_F(GraphTest, EdgeOperations) {
    EXPECT_TRUE(graph.has_edge("P001", "topic:array"));
    EXPECT_TRUE(graph.has_edge("topic:array", "P001"));  // bidirectional
    EXPECT_FALSE(graph.has_edge("P001", "topic:graph"));
}

TEST_F(GraphTest, GetNeighbors) {
    auto neighbors = graph.get_neighbors("P002");
    EXPECT_EQ(neighbors.size(), 3u);  // graph, heap, dijkstra
}

TEST_F(GraphTest, BFS) {
    auto result = graph.bfs("P002", 2);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result[0], "P002");  // start node first

    // Should find P003 through graph topic
    bool found_p003 = false;
    for (const auto& node : result) {
        if (node == "P003") found_p003 = true;
    }
    EXPECT_TRUE(found_p003);
}

TEST_F(GraphTest, BFSMaxDepth) {
    auto depth1 = graph.bfs("P001", 1);
    auto depth2 = graph.bfs("P001", 3);
    EXPECT_LE(depth1.size(), depth2.size());
}

TEST_F(GraphTest, BFSNonexistentNode) {
    auto result = graph.bfs("nonexistent", 2);
    EXPECT_TRUE(result.empty());
}

TEST_F(GraphTest, DFS) {
    auto result = graph.dfs("P002", 2);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result[0], "P002");
}

TEST_F(GraphTest, DFSMaxDepth) {
    auto depth0 = graph.dfs("P002", 0);
    EXPECT_EQ(depth0.size(), 1u);  // only the start node
}

TEST_F(GraphTest, Dijkstra) {
    auto distances = graph.dijkstra("P002");
    EXPECT_EQ(distances["P002"], 0.0);
    EXPECT_GT(distances.count("topic:graph"), 0u);
    EXPECT_EQ(distances["topic:graph"], 1.0);
    // P003 is 2 hops: P002 -> graph -> P003
    EXPECT_EQ(distances["P003"], 2.0);
}

TEST_F(GraphTest, DijkstraNonexistentNode) {
    auto distances = graph.dijkstra("nonexistent");
    EXPECT_TRUE(distances.empty());
}

TEST_F(GraphTest, Similarity) {
    // P002 and P003 share "topic:graph"
    double sim = graph.similarity("P002", "P003");
    EXPECT_GT(sim, 0.0);

    // P001 and P002 share nothing directly
    double sim2 = graph.similarity("P001", "P002");
    EXPECT_EQ(sim2, 0.0);
}

TEST_F(GraphTest, GetConnectedByType) {
    auto problems = graph.get_connected_by_type("topic:graph", "problem", 1);
    EXPECT_EQ(problems.size(), 2u);  // P002 and P003
}

TEST_F(GraphTest, FindRelatedProblems) {
    Problem p002;
    p002.id = "P002";
    p002.title = "Network Delay Time";
    graph.set_problem_data("P002", p002);

    Problem p003;
    p003.id = "P003";
    p003.title = "Course Schedule";
    graph.set_problem_data("P003", p003);

    auto related = graph.find_related_problems("P002", 5);
    EXPECT_FALSE(related.empty());
    // P003 should be related (share graph topic)
    bool found = false;
    for (const auto& r : related) {
        if (r.problem.id == "P003") {
            found = true;
            EXPECT_GT(r.similarity, 0.0);
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(GraphTest, Clear) {
    graph.clear();
    EXPECT_EQ(graph.node_count(), 0u);
}

TEST_F(GraphTest, DuplicateEdge) {
    // Adding same edge twice should not create duplicates
    graph.add_edge("P001", "topic:array", 1.0);
    auto neighbors = graph.get_neighbors("P001");
    int count = 0;
    for (const auto& e : neighbors) {
        if (e.target == "topic:array") count++;
    }
    EXPECT_EQ(count, 1);
}
