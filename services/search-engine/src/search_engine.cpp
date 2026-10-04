#include "algosearch/search_engine.hpp"
#include <algorithm>
#include <set>

namespace algosearch {

SearchEngine::SearchEngine() = default;
SearchEngine::~SearchEngine() = default;

void SearchEngine::load_problems(const std::vector<Problem>& problems) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Clear existing data
    problems_.clear();
    trie_.clear();
    index_.clear();
    graph_.clear();
    topic_index_.clear();
    algorithm_index_.clear();
    difficulty_index_.clear();

    // Index all problems
    for (const auto& problem : problems) {
        index_problem(problem);
    }

    // Build graph relationships
    build_graph();
}

void SearchEngine::add_problem(const Problem& problem) {
    std::lock_guard<std::mutex> lock(mutex_);
    index_problem(problem);
    build_graph();
}

void SearchEngine::index_problem(const Problem& problem) {
    // Store the problem
    problems_[problem.id] = problem;

    // Index in inverted index
    // Title gets higher weight
    auto title_tokens = filter_stop_words(tokenize(problem.title));
    index_.add_document(problem.id, "title", title_tokens, 2.0);

    // Description tokens
    auto desc_tokens = filter_stop_words(tokenize(problem.description));
    index_.add_document(problem.id, "description", desc_tokens, 1.0);

    // Add topics and algorithms as indexed terms with boost
    std::vector<std::string> topic_tokens;
    for (const auto& topic : problem.topics) {
        auto tt = tokenize(topic);
        topic_tokens.insert(topic_tokens.end(), tt.begin(), tt.end());
    }
    index_.add_document(problem.id, "topics", topic_tokens, 1.5);

    std::vector<std::string> algo_tokens;
    for (const auto& algo : problem.algorithms) {
        auto at = tokenize(algo);
        algo_tokens.insert(algo_tokens.end(), at.begin(), at.end());
    }
    index_.add_document(problem.id, "algorithms", algo_tokens, 1.5);

    // Add to trie for autocomplete
    trie_.insert(problem.title, "problem");
    // Add individual important terms
    for (const auto& token : title_tokens) {
        if (token.size() >= 3) {
            trie_.insert(token, "keyword");
        }
    }
    for (const auto& topic : problem.topics) {
        trie_.insert(topic, "topic");
    }
    for (const auto& algo : problem.algorithms) {
        trie_.insert(algo, "algorithm");
    }

    // Topic index
    for (const auto& topic : problem.topics) {
        topic_index_[to_lower(topic)].push_back(problem.id);
    }

    // Algorithm index
    for (const auto& algo : problem.algorithms) {
        algorithm_index_[to_lower(algo)].push_back(problem.id);
    }

    // Difficulty index
    difficulty_index_[to_lower(problem.difficulty)].push_back(problem.id);
}

void SearchEngine::build_graph() {
    graph_.clear();

    // Add all problems as nodes
    for (const auto& [id, problem] : problems_) {
        graph_.add_node(id, "problem", problem.title);
        graph_.set_problem_data(id, problem);
    }

    // Add topic and algorithm nodes
    std::set<std::string> all_topics, all_algorithms;
    for (const auto& [id, problem] : problems_) {
        for (const auto& topic : problem.topics) {
            all_topics.insert(topic);
        }
        for (const auto& algo : problem.algorithms) {
            all_algorithms.insert(algo);
        }
    }

    for (const auto& topic : all_topics) {
        std::string node_id = "topic:" + to_lower(topic);
        graph_.add_node(node_id, "topic", topic);
    }

    for (const auto& algo : all_algorithms) {
        std::string node_id = "algo:" + to_lower(algo);
        graph_.add_node(node_id, "algorithm", algo);
    }

    // Connect problems to their topics and algorithms
    for (const auto& [id, problem] : problems_) {
        for (const auto& topic : problem.topics) {
            std::string topic_id = "topic:" + to_lower(topic);
            graph_.add_bidirectional_edge(id, topic_id, 1.0);
        }
        for (const auto& algo : problem.algorithms) {
            std::string algo_id = "algo:" + to_lower(algo);
            graph_.add_bidirectional_edge(id, algo_id, 1.0);
        }
    }

    // Connect related topics (e.g., Graph-BFS, Graph-DFS, Tree-DFS)
    // This creates natural recommendation paths
    std::unordered_map<std::string, std::string> topic_relationships = {
        {"topic:graph", "algo:bfs"},
        {"topic:graph", "algo:dfs"},
        {"topic:graph", "algo:dijkstra"},
        {"topic:tree", "algo:dfs"},
        {"topic:tree", "algo:bfs"},
        {"topic:sorting", "algo:merge sort"},
        {"topic:sorting", "algo:quick sort"},
        {"topic:dynamic programming", "topic:greedy"},
        {"topic:heap", "algo:dijkstra"},
        {"topic:trie", "topic:string"},
    };

    for (const auto& [from, to] : topic_relationships) {
        if (graph_.has_node(from) && graph_.has_node(to)) {
            graph_.add_bidirectional_edge(from, to, 0.5);
        }
    }
}

const Problem* SearchEngine::get_problem(const std::string& id) const {
    auto it = problems_.find(id);
    return it != problems_.end() ? &it->second : nullptr;
}

std::vector<Problem> SearchEngine::get_all_problems() const {
    std::vector<Problem> result;
    result.reserve(problems_.size());
    for (const auto& [id, problem] : problems_) {
        result.push_back(problem);
    }
    return result;
}

size_t SearchEngine::problem_count() const {
    return problems_.size();
}

SearchResponse SearchEngine::search(const SearchQuery& query) const {
    Timer timer;
    SearchResponse response;
    response.query = query.query;

    if (query.query.empty()) {
        // No query = browse mode, return filtered results
        std::vector<std::pair<Problem, double>> all_candidates;
        for (const auto& [id, problem] : problems_) {
            all_candidates.emplace_back(problem, 0.0);
        }

        auto ranked = ranker_.rank(all_candidates, {}, query.difficulty);

        // Apply filters
        std::vector<ScoredResult> filtered;
        for (auto& r : ranked) {
            bool pass = true;
            if (!query.difficulty.empty() && to_lower(r.problem.difficulty) != to_lower(query.difficulty)) pass = false;
            if (!query.topic.empty()) {
                bool has_topic = false;
                for (const auto& t : r.problem.topics) {
                    if (to_lower(t) == to_lower(query.topic)) { has_topic = true; break; }
                }
                if (!has_topic) pass = false;
            }
            if (!query.algorithm.empty()) {
                bool has_algo = false;
                for (const auto& a : r.problem.algorithms) {
                    if (to_lower(a) == to_lower(query.algorithm)) { has_algo = true; break; }
                }
                if (!has_algo) pass = false;
            }
            if (!query.platform.empty() && to_lower(r.problem.platform) != to_lower(query.platform)) pass = false;

            if (pass) filtered.push_back(std::move(r));
        }

        response.total_count = static_cast<int>(filtered.size());

        // Apply pagination
        int start = query.offset;
        int end = std::min(start + query.limit, static_cast<int>(filtered.size()));
        if (start < static_cast<int>(filtered.size())) {
            response.results.assign(filtered.begin() + start, filtered.begin() + end);
        }

        response.search_duration_ms = timer.elapsed_ms();
        return response;
    }

    // Tokenize and filter query
    auto query_tokens = filter_stop_words(tokenize(query.query));
    if (query_tokens.empty()) {
        // All stop words - try raw tokenization
        query_tokens = tokenize(query.query);
    }

    // Step 1: Use inverted index to get candidate documents with TF-IDF scores
    auto scored_docs = index_.search_scored(query_tokens);

    // Step 2: Also try fuzzy matching for typo tolerance on low-result queries
    if (scored_docs.size() < 5) {
        // Try each token with fuzzy matching against indexed terms
        for (const auto& [id, problem] : problems_) {
            if (scored_docs.count(id)) continue;

            double fuzzy_score = StringMatcher::fuzzy_token_match_score(
                query_tokens, problem.title, 2);
            if (fuzzy_score > 0.3) {
                scored_docs[id] = fuzzy_score * 0.5;  // lower weight for fuzzy
            }
        }
    }

    // Step 3: Prepare candidates for ranking
    std::vector<std::pair<Problem, double>> candidates;
    for (const auto& [doc_id, tfidf_score] : scored_docs) {
        auto pit = problems_.find(doc_id);
        if (pit == problems_.end()) continue;
        candidates.emplace_back(pit->second, tfidf_score);
    }

    // Step 4: Apply full ranking
    auto ranked = ranker_.rank(candidates, query_tokens, query.difficulty);

    // Step 5: Apply filters
    std::vector<ScoredResult> filtered;
    for (auto& r : ranked) {
        bool pass = true;
        if (!query.difficulty.empty() && to_lower(r.problem.difficulty) != to_lower(query.difficulty)) pass = false;
        if (!query.topic.empty()) {
            bool has_topic = false;
            for (const auto& t : r.problem.topics) {
                if (to_lower(t) == to_lower(query.topic)) { has_topic = true; break; }
            }
            if (!has_topic) pass = false;
        }
        if (!query.algorithm.empty()) {
            bool has_algo = false;
            for (const auto& a : r.problem.algorithms) {
                if (to_lower(a) == to_lower(query.algorithm)) { has_algo = true; break; }
            }
            if (!has_algo) pass = false;
        }
        if (!query.platform.empty() && to_lower(r.problem.platform) != to_lower(query.platform)) pass = false;

        if (pass) filtered.push_back(std::move(r));
    }

    // Step 6: Top-K retrieval using min-heap
    TopKRetriever topk(query.offset + query.limit);
    topk.push_batch(filtered);
    auto topk_results = topk.get_results();

    response.total_count = static_cast<int>(filtered.size());

    // Apply pagination offset
    int start = query.offset;
    int end = std::min(start + query.limit, static_cast<int>(topk_results.size()));
    if (start < static_cast<int>(topk_results.size())) {
        response.results.assign(topk_results.begin() + start, topk_results.begin() + end);
    }

    // Step 7: Sort final results
    if (query.sort_by == "popularity") {
        std::sort(response.results.begin(), response.results.end(),
            [](const ScoredResult& a, const ScoredResult& b) {
                return a.problem.popularity > b.problem.popularity;
            });
    } else if (query.sort_by == "recent") {
        std::sort(response.results.begin(), response.results.end(),
            [](const ScoredResult& a, const ScoredResult& b) {
                return a.problem.created_at > b.problem.created_at;
            });
    }
    // "relevance" keeps the existing ranking order

    response.search_duration_ms = timer.elapsed_ms();
    return response;
}

std::vector<AutocompleteResult> SearchEngine::autocomplete(
    const std::string& prefix,
    int limit
) const {
    auto suggestions = trie_.autocomplete(prefix, limit);

    std::vector<AutocompleteResult> results;
    results.reserve(suggestions.size());

    for (const auto& s : suggestions) {
        results.push_back({s.term, s.frequency, s.category});
    }

    return results;
}

std::vector<RelatedProblem> SearchEngine::get_related_problems(
    const std::string& problem_id,
    int limit
) const {
    return graph_.find_related_problems(problem_id, limit);
}

std::vector<std::string> SearchEngine::get_topics() const {
    std::vector<std::string> topics;
    for (const auto& [topic, _] : topic_index_) {
        topics.push_back(topic);
    }
    std::sort(topics.begin(), topics.end());
    return topics;
}

std::vector<std::string> SearchEngine::get_algorithms() const {
    std::vector<std::string> algorithms;
    for (const auto& [algo, _] : algorithm_index_) {
        algorithms.push_back(algo);
    }
    std::sort(algorithms.begin(), algorithms.end());
    return algorithms;
}

std::vector<Problem> SearchEngine::get_problems_by_topic(const std::string& topic) const {
    std::string key = to_lower(topic);
    auto it = topic_index_.find(key);
    if (it == topic_index_.end()) return {};

    std::vector<Problem> result;
    for (const auto& id : it->second) {
        auto pit = problems_.find(id);
        if (pit != problems_.end()) {
            result.push_back(pit->second);
        }
    }
    return result;
}

std::vector<Problem> SearchEngine::get_problems_by_algorithm(const std::string& algorithm) const {
    std::string key = to_lower(algorithm);
    auto it = algorithm_index_.find(key);
    if (it == algorithm_index_.end()) return {};

    std::vector<Problem> result;
    for (const auto& id : it->second) {
        auto pit = problems_.find(id);
        if (pit != problems_.end()) {
            result.push_back(pit->second);
        }
    }
    return result;
}

std::vector<Problem> SearchEngine::get_problems_by_difficulty(const std::string& difficulty) const {
    std::string key = to_lower(difficulty);
    auto it = difficulty_index_.find(key);
    if (it == difficulty_index_.end()) return {};

    std::vector<Problem> result;
    for (const auto& id : it->second) {
        auto pit = problems_.find(id);
        if (pit != problems_.end()) {
            result.push_back(pit->second);
        }
    }
    return result;
}

SearchEngine::EngineStats SearchEngine::get_stats() const {
    return {
        problems_.size(),
        index_.term_count(),
        trie_.size(),
        graph_.node_count(),
        topic_index_.size(),
        algorithm_index_.size()
    };
}

void SearchEngine::track_search(const std::string& query) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto tokens = tokenize(query);
    for (const auto& t : tokens) {
        trie_.increment_frequency(t);
    }
}

} // namespace algosearch
