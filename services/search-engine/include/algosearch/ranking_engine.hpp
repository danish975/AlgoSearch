#pragma once

#include "types.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace algosearch {

/**
 * Configurable relevance ranking engine.
 * 
 * Combines multiple scoring signals into a final relevance score.
 * Signals are weighted and normalized before combination.
 * 
 * Default weight configuration:
 *   Title match:       0.35
 *   Keyword relevance: 0.25
 *   Topic similarity:  0.15
 *   Difficulty match:  0.10
 *   Popularity:        0.10
 *   Recency:           0.05
 * 
 * These are initial design values. Actual search quality should be
 * evaluated with curated query sets and ranking metrics (Precision@K, NDCG@K).
 */
class RankingEngine {
public:
    struct Weights {
        double title_match = 0.35;
        double keyword_relevance = 0.25;
        double topic_similarity = 0.15;
        double difficulty_match = 0.10;
        double popularity = 0.10;
        double recency = 0.05;
    };

    RankingEngine();
    explicit RankingEngine(const Weights& weights);

    // Score a single problem against a search query
    ScoredResult score(
        const Problem& problem,
        const std::vector<std::string>& query_tokens,
        const std::string& difficulty_filter = "",
        double index_score = 0.0
    ) const;

    // Score and rank a batch of problems
    std::vector<ScoredResult> rank(
        const std::vector<std::pair<Problem, double>>& candidates,
        const std::vector<std::string>& query_tokens,
        const std::string& difficulty_filter = ""
    ) const;

    // Get current weights
    const Weights& get_weights() const;

    // Update weights
    void set_weights(const Weights& weights);

private:
    Weights weights_;

    // Individual scoring functions (each returns 0.0 to 1.0)
    double score_title_match(
        const Problem& problem,
        const std::vector<std::string>& query_tokens
    ) const;

    double score_keyword_relevance(
        const Problem& problem,
        const std::vector<std::string>& query_tokens,
        double index_score
    ) const;

    double score_topic_similarity(
        const Problem& problem,
        const std::vector<std::string>& query_tokens
    ) const;

    double score_difficulty_match(
        const Problem& problem,
        const std::string& difficulty_filter
    ) const;

    double score_popularity(const Problem& problem) const;

    double score_recency(const Problem& problem) const;

    // Normalize a raw score to 0.0-1.0 range
    double normalize(double value, double max_value) const;
};

} // namespace algosearch
