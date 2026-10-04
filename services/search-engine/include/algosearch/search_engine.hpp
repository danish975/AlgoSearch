#pragma once

#include "types.hpp"
#include "trie.hpp"
#include "inverted_index.hpp"
#include "min_heap.hpp"
#include "graph.hpp"
#include "string_matcher.hpp"
#include "ranking_engine.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace algosearch {

/**
 * Search engine orchestrator.
 * 
 * Coordinates all DSA components: Trie, Inverted Index, Min-Heap,
 * Graph, String Matching, and Ranking Engine.
 * 
 * Thread-safe for concurrent read operations after initialization.
 * Write operations (load_problems, add_problem) acquire a write lock.
 */
class SearchEngine {
public:
    SearchEngine();
    ~SearchEngine();

    // ── Data management ────────────────────────────────────────────
    
    // Load problems into all indexes
    void load_problems(const std::vector<Problem>& problems);

    // Add a single problem to all indexes
    void add_problem(const Problem& problem);

    // Get problem by ID
    const Problem* get_problem(const std::string& id) const;

    // Get all problems
    std::vector<Problem> get_all_problems() const;

    // Get total problem count
    size_t problem_count() const;

    // ── Search ─────────────────────────────────────────────────────

    // Full search with all features: index retrieval, ranking, filtering, top-K
    SearchResponse search(const SearchQuery& query) const;

    // ── Autocomplete ───────────────────────────────────────────────

    // Prefix-based autocomplete suggestions
    std::vector<AutocompleteResult> autocomplete(
        const std::string& prefix,
        int limit = 8
    ) const;

    // ── Recommendations ────────────────────────────────────────────

    // Find problems related to a given problem using graph
    std::vector<RelatedProblem> get_related_problems(
        const std::string& problem_id,
        int limit = 10
    ) const;

    // ── Metadata ───────────────────────────────────────────────────

    // Get all unique topics
    std::vector<std::string> get_topics() const;

    // Get all unique algorithms
    std::vector<std::string> get_algorithms() const;

    // Get problems by topic
    std::vector<Problem> get_problems_by_topic(const std::string& topic) const;

    // Get problems by algorithm
    std::vector<Problem> get_problems_by_algorithm(const std::string& algorithm) const;

    // Get problems by difficulty
    std::vector<Problem> get_problems_by_difficulty(const std::string& difficulty) const;

    // ── Statistics ─────────────────────────────────────────────────

    struct EngineStats {
        size_t total_problems;
        size_t index_terms;
        size_t trie_terms;
        size_t graph_nodes;
        size_t topics_count;
        size_t algorithms_count;
    };

    EngineStats get_stats() const;

    // Track a search query for popularity
    void track_search(const std::string& query);

private:
    // Core DSA components
    Trie trie_;
    InvertedIndex index_;
    Graph graph_;
    RankingEngine ranker_;

    // Problem storage
    std::unordered_map<std::string, Problem> problems_;

    // Topic/algorithm indexes
    std::unordered_map<std::string, std::vector<std::string>> topic_index_;
    std::unordered_map<std::string, std::vector<std::string>> algorithm_index_;
    std::unordered_map<std::string, std::vector<std::string>> difficulty_index_;

    // Thread safety
    mutable std::mutex mutex_;

    // Index a single problem into all data structures
    void index_problem(const Problem& problem);

    // Build graph relationships from problem data
    void build_graph();

    // Apply filters to candidate set
    std::vector<std::string> apply_filters(
        const std::vector<std::string>& candidates,
        const SearchQuery& query
    ) const;
};

} // namespace algosearch
