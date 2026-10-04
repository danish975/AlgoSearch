#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <nlohmann/json.hpp>

namespace algosearch {

// ── Core data structures ────────────────────────────────────────────

struct Problem {
    std::string id;
    std::string title;
    std::string slug;
    std::string description;
    std::string difficulty;  // "Easy", "Medium", "Hard"
    std::vector<std::string> topics;
    std::vector<std::string> algorithms;
    std::string platform;
    std::string external_url;
    double popularity = 0.0;
    std::string created_at;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        Problem, id, title, slug, description, difficulty,
        topics, algorithms, platform, external_url, popularity, created_at
    )
};

struct SearchQuery {
    std::string query;
    std::string difficulty;
    std::string topic;
    std::string algorithm;
    std::string platform;
    std::string sort_by = "relevance";  // relevance, popularity, recent
    int limit = 10;
    int offset = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        SearchQuery, query, difficulty, topic, algorithm,
        platform, sort_by, limit, offset
    )
};

struct ScoredResult {
    Problem problem;
    double score = 0.0;
    std::unordered_map<std::string, double> score_breakdown;

    bool operator<(const ScoredResult& other) const {
        if (score != other.score) return score < other.score;
        return problem.id > other.id;  // deterministic tie-breaking
    }

    bool operator>(const ScoredResult& other) const {
        if (score != other.score) return score > other.score;
        return problem.id < other.id;
    }
};

inline void to_json(nlohmann::json& j, const ScoredResult& r) {
    j = nlohmann::json{
        {"problem", r.problem},
        {"score", r.score},
        {"score_breakdown", r.score_breakdown}
    };
}

struct AutocompleteResult {
    std::string term;
    int frequency = 0;
    std::string category;  // "problem", "topic", "algorithm"
};

inline void to_json(nlohmann::json& j, const AutocompleteResult& r) {
    j = nlohmann::json{
        {"term", r.term},
        {"frequency", r.frequency},
        {"category", r.category}
    };
}

struct SearchResponse {
    std::vector<ScoredResult> results;
    int total_count = 0;
    double search_duration_ms = 0.0;
    std::string query;
};

inline void to_json(nlohmann::json& j, const SearchResponse& r) {
    j = nlohmann::json{
        {"results", r.results},
        {"total_count", r.total_count},
        {"search_duration_ms", r.search_duration_ms},
        {"query", r.query}
    };
}

struct RelatedProblem {
    Problem problem;
    double similarity = 0.0;
    std::vector<std::string> shared_topics;
};

inline void to_json(nlohmann::json& j, const RelatedProblem& r) {
    j = nlohmann::json{
        {"problem", r.problem},
        {"similarity", r.similarity},
        {"shared_topics", r.shared_topics}
    };
}

// ── Utility ─────────────────────────────────────────────────────────

inline std::string to_lower(const std::string& s) {
    std::string result = s;
    for (auto& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

inline std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string current;
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            current += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        } else if (!current.empty()) {
            tokens.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

// Common stop words to filter from indexing
inline const std::unordered_map<std::string, bool>& stop_words() {
    static const std::unordered_map<std::string, bool> words = {
        {"a", true}, {"an", true}, {"the", true}, {"is", true},
        {"it", true}, {"in", true}, {"on", true}, {"at", true},
        {"to", true}, {"of", true}, {"for", true}, {"and", true},
        {"or", true}, {"but", true}, {"not", true}, {"with", true},
        {"by", true}, {"from", true}, {"as", true}, {"be", true},
        {"was", true}, {"are", true}, {"been", true}, {"has", true},
        {"have", true}, {"had", true}, {"do", true}, {"does", true},
        {"did", true}, {"will", true}, {"would", true}, {"could", true},
        {"should", true}, {"may", true}, {"might", true}, {"can", true},
        {"this", true}, {"that", true}, {"these", true}, {"those", true},
        {"i", true}, {"you", true}, {"he", true}, {"she", true},
        {"we", true}, {"they", true}, {"me", true}, {"him", true},
        {"her", true}, {"us", true}, {"them", true}, {"my", true},
        {"your", true}, {"his", true}, {"its", true}, {"our", true},
        {"their", true}, {"what", true}, {"which", true}, {"who", true},
        {"when", true}, {"where", true}, {"how", true}, {"all", true},
        {"each", true}, {"every", true}, {"both", true}, {"few", true},
        {"more", true}, {"most", true}, {"other", true}, {"some", true},
        {"such", true}, {"no", true}, {"nor", true}, {"only", true},
        {"own", true}, {"same", true}, {"so", true}, {"than", true},
        {"too", true}, {"very", true}, {"just", true}, {"if", true},
    };
    return words;
}

inline bool is_stop_word(const std::string& word) {
    return stop_words().count(word) > 0;
}

inline std::vector<std::string> filter_stop_words(const std::vector<std::string>& tokens) {
    std::vector<std::string> result;
    for (const auto& t : tokens) {
        if (!is_stop_word(t)) {
            result.push_back(t);
        }
    }
    return result;
}

// Timer helper for measuring search duration
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    double elapsed_ms() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }

    void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // namespace algosearch
