#pragma once

#include "types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <limits>

namespace algosearch {

/**
 * Graph-based recommendation engine using adjacency list representation.
 * 
 * Models relationships between problems, topics, and algorithms.
 * Uses BFS/DFS for traversal and Dijkstra for weighted shortest paths.
 * 
 * Node types: "problem", "topic", "algorithm"
 * Edge weights represent relationship strength (lower = stronger).
 * 
 * Complexity:
 *   BFS/DFS:    O(V + E)
 *   Dijkstra:   O((V + E) log V) with priority queue
 *   Add edge:   O(1) amortized
 */
class Graph {
public:
    struct Edge {
        std::string target;
        double weight;
    };

    struct Node {
        std::string id;
        std::string type;  // "problem", "topic", "algorithm"
        std::string label;
        std::unordered_map<std::string, std::string> metadata;
    };

    Graph();
    ~Graph();

    // ── Node operations ────────────────────────────────────────────
    void add_node(const std::string& id, const std::string& type, const std::string& label);
    bool has_node(const std::string& id) const;
    const Node* get_node(const std::string& id) const;
    size_t node_count() const;

    // ── Edge operations ────────────────────────────────────────────
    void add_edge(const std::string& from, const std::string& to, double weight = 1.0);
    void add_bidirectional_edge(const std::string& from, const std::string& to, double weight = 1.0);
    bool has_edge(const std::string& from, const std::string& to) const;
    std::vector<Edge> get_neighbors(const std::string& node_id) const;

    // ── Traversal ──────────────────────────────────────────────────
    
    // BFS from a start node, returns nodes in BFS order up to max_depth
    std::vector<std::string> bfs(const std::string& start, int max_depth = 3) const;

    // DFS from a start node, returns nodes in DFS order up to max_depth
    std::vector<std::string> dfs(const std::string& start, int max_depth = 3) const;

    // Dijkstra's shortest path from start to all reachable nodes
    // Returns map of node_id -> shortest distance
    std::unordered_map<std::string, double> dijkstra(const std::string& start) const;

    // ── Recommendation ─────────────────────────────────────────────

    // Find related problems for a given problem using graph proximity
    // Traverses topic/algorithm connections to find nearby problems
    std::vector<RelatedProblem> find_related_problems(
        const std::string& problem_id,
        int limit = 10,
        int max_depth = 3
    ) const;

    // Calculate similarity between two nodes based on shared neighbors
    double similarity(const std::string& node_a, const std::string& node_b) const;

    // Get all nodes of a given type connected to a node
    std::vector<std::string> get_connected_by_type(
        const std::string& node_id,
        const std::string& type,
        int max_depth = 2
    ) const;

    // ── Utility ────────────────────────────────────────────────────
    void clear();

    // Store problem data for recommendation results
    void set_problem_data(const std::string& problem_id, const Problem& problem);

private:
    std::unordered_map<std::string, Node> nodes_;
    std::unordered_map<std::string, std::vector<Edge>> adjacency_;
    std::unordered_map<std::string, Problem> problem_data_;

    // DFS helper with depth tracking
    void dfs_helper(
        const std::string& node,
        int depth,
        int max_depth,
        std::unordered_set<std::string>& visited,
        std::vector<std::string>& result
    ) const;
};

} // namespace algosearch
