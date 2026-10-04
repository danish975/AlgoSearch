#include "algosearch/ranking_engine.hpp"
#include "algosearch/string_matcher.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace algosearch {

RankingEngine::RankingEngine() = default;
RankingEngine::RankingEngine(const Weights& weights) : weights_(weights) {}

ScoredResult RankingEngine::score(
    const Problem& problem,
    const std::vector<std::string>& query_tokens,
    const std::string& difficulty_filter,
    double index_score
) const {
    ScoredResult result;
    result.problem = problem;

    double title = score_title_match(problem, query_tokens);
    double keyword = score_keyword_relevance(problem, query_tokens, index_score);
    double topic = score_topic_similarity(problem, query_tokens);
    double diff = score_difficulty_match(problem, difficulty_filter);
    double pop = score_popularity(problem);
    double rec = score_recency(problem);

    result.score =
        weights_.title_match * title +
        weights_.keyword_relevance * keyword +
        weights_.topic_similarity * topic +
        weights_.difficulty_match * diff +
        weights_.popularity * pop +
        weights_.recency * rec;

    // Store breakdown for debugging/transparency
    result.score_breakdown["title_match"] = title;
    result.score_breakdown["keyword_relevance"] = keyword;
    result.score_breakdown["topic_similarity"] = topic;
    result.score_breakdown["difficulty_match"] = diff;
    result.score_breakdown["popularity"] = pop;
    result.score_breakdown["recency"] = rec;

    return result;
}

std::vector<ScoredResult> RankingEngine::rank(
    const std::vector<std::pair<Problem, double>>& candidates,
    const std::vector<std::string>& query_tokens,
    const std::string& difficulty_filter
) const {
    std::vector<ScoredResult> results;
    results.reserve(candidates.size());

    for (const auto& [problem, index_score] : candidates) {
        results.push_back(score(problem, query_tokens, difficulty_filter, index_score));
    }

    // Sort by score descending, with deterministic tie-breaking by ID
    std::sort(results.begin(), results.end(),
        [](const ScoredResult& a, const ScoredResult& b) {
            if (a.score != b.score) return a.score > b.score;
            return a.problem.id < b.problem.id;
        });

    return results;
}

const RankingEngine::Weights& RankingEngine::get_weights() const {
    return weights_;
}

void RankingEngine::set_weights(const Weights& weights) {
    weights_ = weights;
}

double RankingEngine::score_title_match(
    const Problem& problem,
    const std::vector<std::string>& query_tokens
) const {
    if (query_tokens.empty()) return 0.0;

    // Combine exact match + fuzzy match for robust title scoring
    double exact = StringMatcher::token_match_score(query_tokens, problem.title);
    double fuzzy = StringMatcher::fuzzy_token_match_score(query_tokens, problem.title, 2);

    // Also check if the full query appears as a substring in the title
    std::string query_str;
    for (const auto& t : query_tokens) {
        if (!query_str.empty()) query_str += " ";
        query_str += t;
    }

    double substring_bonus = 0.0;
    if (StringMatcher::kmp_contains(problem.title, query_str)) {
        substring_bonus = 0.3;
    }

    return std::min(1.0, std::max(exact, fuzzy) + substring_bonus);
}

double RankingEngine::score_keyword_relevance(
    const Problem& problem,
    const std::vector<std::string>& query_tokens,
    double index_score
) const {
    if (query_tokens.empty()) return 0.0;

    // Use the TF-IDF score from inverted index as base
    double base_score = index_score;

    // Also check description match
    double desc_score = StringMatcher::token_match_score(query_tokens, problem.description);

    // Normalize: assume max reasonable TF-IDF is ~10
    double normalized_index = normalize(base_score, 10.0);

    return std::max(normalized_index, desc_score * 0.7);
}

double RankingEngine::score_topic_similarity(
    const Problem& problem,
    const std::vector<std::string>& query_tokens
) const {
    if (query_tokens.empty() || problem.topics.empty()) return 0.0;

    int matches = 0;
    for (const auto& qt : query_tokens) {
        for (const auto& topic : problem.topics) {
            std::string lower_topic = to_lower(topic);
            if (lower_topic == qt || lower_topic.find(qt) != std::string::npos) {
                matches++;
                break;
            }
        }
        // Also check algorithms
        for (const auto& algo : problem.algorithms) {
            std::string lower_algo = to_lower(algo);
            if (lower_algo == qt || lower_algo.find(qt) != std::string::npos) {
                matches++;
                break;
            }
        }
    }

    return static_cast<double>(matches) /
           static_cast<double>(query_tokens.size());
}

double RankingEngine::score_difficulty_match(
    const Problem& problem,
    const std::string& difficulty_filter
) const {
    if (difficulty_filter.empty()) return 0.5;  // neutral when no filter
    return (to_lower(problem.difficulty) == to_lower(difficulty_filter)) ? 1.0 : 0.0;
}

double RankingEngine::score_popularity(const Problem& problem) const {
    // Normalize popularity to 0-1 (assume max popularity of 1000)
    return normalize(problem.popularity, 1000.0);
}

double RankingEngine::score_recency(const Problem& problem) const {
    if (problem.created_at.empty()) return 0.0;

    // Parse ISO date and compute age
    // Simple approach: extract year and compare to current
    try {
        // Format: YYYY-MM-DDTHH:MM:SS.000Z
        int year = std::stoi(problem.created_at.substr(0, 4));

        // Get current year
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        struct tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &time_t);
#else
        localtime_r(&time_t, &tm_buf);
#endif
        int current_year = tm_buf.tm_year + 1900;

        int age_years = current_year - year;
        // Newer = higher score, decay over 5 years
        return std::max(0.0, 1.0 - static_cast<double>(age_years) / 5.0);
    } catch (...) {
        return 0.0;
    }
}

double RankingEngine::normalize(double value, double max_value) const {
    if (max_value <= 0.0) return 0.0;
    return std::min(1.0, std::max(0.0, value / max_value));
}

} // namespace algosearch
