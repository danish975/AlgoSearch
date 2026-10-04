#include "algosearch/graph.hpp"
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>

namespace algosearch {

Graph::Graph() = default;
Graph::~Graph() = default;

void Graph::add_node(const std::string& id, const std::string& type, const std::string& label) {
    if (nodes_.count(id)) return;
    nodes_[id] = {id, type, label, {}};
    adjacency_[id]; // ensure adjacency list exists
}

bool Graph::has_node(const std::string& id) const {
    return nodes_.count(id) > 0;
}

const Graph::Node* Graph::get_node(const std::string& id) const {
    auto it = nodes_.find(id);
    return it != nodes_.end() ? &it->second : nullptr;
}

size_t Graph::node_count() const {
    return nodes_.size();
}

void Graph::add_edge(const std::string& from, const std::string& to, double weight) {
    if (!has_node(from) || !has_node(to)) return;

    // Check for duplicate edge
    auto& edges = adjacency_[from];
    for (const auto& e : edges) {
        if (e.target == to) return;
    }

    edges.push_back({to, weight});
}

void Graph::add_bidirectional_edge(const std::string& from, const std::string& to, double weight) {
    add_edge(from, to, weight);
    add_edge(to, from, weight);
}

bool Graph::has_edge(const std::string& from, const std::string& to) const {
    auto it = adjacency_.find(from);
    if (it == adjacency_.end()) return false;
    for (const auto& e : it->second) {
        if (e.target == to) return true;
    }
    return false;
}

std::vector<Graph::Edge> Graph::get_neighbors(const std::string& node_id) const {
    auto it = adjacency_.find(node_id);
    if (it == adjacency_.end()) return {};
    return it->second;
}

std::vector<std::string> Graph::bfs(const std::string& start, int max_depth) const {
    if (!has_node(start)) return {};

    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    std::queue<std::pair<std::string, int>> q;

    q.push({start, 0});
    visited.insert(start);

    while (!q.empty()) {
        auto [node, depth] = q.front();
        q.pop();

        result.push_back(node);

        if (depth >= max_depth) continue;

        auto it = adjacency_.find(node);
        if (it == adjacency_.end()) continue;

        for (const auto& edge : it->second) {
            if (!visited.count(edge.target)) {
                visited.insert(edge.target);
                q.push({edge.target, depth + 1});
            }
        }
    }

    return result;
}

std::vector<std::string> Graph::dfs(const std::string& start, int max_depth) const {
    if (!has_node(start)) return {};

    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    dfs_helper(start, 0, max_depth, visited, result);
    return result;
}

void Graph::dfs_helper(
    const std::string& node,
    int depth,
    int max_depth,
    std::unordered_set<std::string>& visited,
    std::vector<std::string>& result
) const {
    visited.insert(node);
    result.push_back(node);

    if (depth >= max_depth) return;

    auto it = adjacency_.find(node);
    if (it == adjacency_.end()) return;

    for (const auto& edge : it->second) {
        if (!visited.count(edge.target)) {
            dfs_helper(edge.target, depth + 1, max_depth, visited, result);
        }
    }
}

std::unordered_map<std::string, double> Graph::dijkstra(const std::string& start) const {
    if (!has_node(start)) return {};

    std::unordered_map<std::string, double> distances;
    for (const auto& [id, _] : nodes_) {
        distances[id] = std::numeric_limits<double>::infinity();
    }
    distances[start] = 0.0;

    // Min-heap: (distance, node_id)
    using PQEntry = std::pair<double, std::string>;
    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pq;
    pq.push({0.0, start});

    std::unordered_set<std::string> processed;

    while (!pq.empty()) {
        auto [dist, node] = pq.top();
        pq.pop();

        if (processed.count(node)) continue;
        processed.insert(node);

        auto it = adjacency_.find(node);
        if (it == adjacency_.end()) continue;

        for (const auto& edge : it->second) {
            double new_dist = dist + edge.weight;
            if (new_dist < distances[edge.target]) {
                distances[edge.target] = new_dist;
                pq.push({new_dist, edge.target});
            }
        }
    }

    // Remove unreachable nodes (infinite distance)
    std::unordered_map<std::string, double> reachable;
    for (const auto& [id, dist] : distances) {
        if (dist < std::numeric_limits<double>::infinity()) {
            reachable[id] = dist;
        }
    }

    return reachable;
}

std::vector<RelatedProblem> Graph::find_related_problems(
    const std::string& problem_id,
    int limit,
    int max_depth
) const {
    if (!has_node(problem_id)) return {};

    // Use Dijkstra to find nearby nodes
    auto distances = dijkstra(problem_id);

    // Collect problem nodes sorted by distance
    std::vector<std::pair<std::string, double>> problem_candidates;
    for (const auto& [node_id, dist] : distances) {
        if (node_id == problem_id) continue;

        auto node = get_node(node_id);
        if (!node || node->type != "problem") continue;

        // Only include within max_depth hops (approximate via distance)
        if (dist <= static_cast<double>(max_depth)) {
            problem_candidates.push_back({node_id, dist});
        }
    }

    // Sort by distance (closest = most related)
    std::sort(problem_candidates.begin(), problem_candidates.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    // Convert to RelatedProblem results
    std::vector<RelatedProblem> results;

    // Get source problem's topics for shared_topics calculation
    std::unordered_set<std::string> source_topics;
    auto src_neighbors = get_neighbors(problem_id);
    for (const auto& edge : src_neighbors) {
        auto node = get_node(edge.target);
        if (node && node->type == "topic") {
            source_topics.insert(node->label);
        }
    }

    for (const auto& [cand_id, dist] : problem_candidates) {
        if (static_cast<int>(results.size()) >= limit) break;

        RelatedProblem rp;
        auto pd_it = problem_data_.find(cand_id);
        if (pd_it != problem_data_.end()) {
            rp.problem = pd_it->second;
        } else {
            auto node = get_node(cand_id);
            if (node) {
                rp.problem.id = node->id;
                rp.problem.title = node->label;
            }
        }

        // Similarity: inverse of distance, normalized to 0-1
        rp.similarity = 1.0 / (1.0 + dist);

        // Find shared topics
        auto cand_neighbors = get_neighbors(cand_id);
        for (const auto& edge : cand_neighbors) {
            auto node = get_node(edge.target);
            if (node && node->type == "topic" && source_topics.count(node->label)) {
                rp.shared_topics.push_back(node->label);
            }
        }

        results.push_back(std::move(rp));
    }

    return results;
}

double Graph::similarity(const std::string& node_a, const std::string& node_b) const {
    if (!has_node(node_a) || !has_node(node_b)) return 0.0;

    // Jaccard similarity based on shared neighbors
    auto neighbors_a = get_neighbors(node_a);
    auto neighbors_b = get_neighbors(node_b);

    std::unordered_set<std::string> set_a, set_b;
    for (const auto& e : neighbors_a) set_a.insert(e.target);
    for (const auto& e : neighbors_b) set_b.insert(e.target);

    if (set_a.empty() && set_b.empty()) return 0.0;

    int intersection = 0;
    for (const auto& n : set_a) {
        if (set_b.count(n)) intersection++;
    }

    int union_size = static_cast<int>(set_a.size() + set_b.size()) - intersection;
    return union_size > 0 ? static_cast<double>(intersection) / union_size : 0.0;
}

std::vector<std::string> Graph::get_connected_by_type(
    const std::string& node_id,
    const std::string& type,
    int max_depth
) const {
    auto all = bfs(node_id, max_depth);
    std::vector<std::string> result;
    for (const auto& id : all) {
        if (id == node_id) continue;
        auto node = get_node(id);
        if (node && node->type == type) {
            result.push_back(id);
        }
    }
    return result;
}

void Graph::clear() {
    nodes_.clear();
    adjacency_.clear();
    problem_data_.clear();
}

void Graph::set_problem_data(const std::string& problem_id, const Problem& problem) {
    problem_data_[problem_id] = problem;
}

} // namespace algosearch
