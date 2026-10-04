#include "algosearch/inverted_index.hpp"
#include <algorithm>
#include <cmath>

namespace algosearch {

InvertedIndex::InvertedIndex() = default;
InvertedIndex::~InvertedIndex() = default;

void InvertedIndex::add_document(
    const std::string& doc_id,
    const std::string& field,
    const std::vector<std::string>& tokens,
    double field_weight
) {
    // Track if this is a new document
    if (doc_terms_.find(doc_id) == doc_terms_.end()) {
        doc_count_++;
        doc_lengths_[doc_id] = 0;
    }

    // Count term frequencies in this batch
    std::unordered_map<std::string, int> local_freq;
    for (const auto& token : tokens) {
        local_freq[token]++;
        doc_terms_[doc_id].insert(token);
    }

    doc_lengths_[doc_id] += static_cast<int>(tokens.size());

    // Update posting lists
    for (const auto& [term, freq] : local_freq) {
        auto& postings = index_[term];

        // Check if this document already has a posting for this term
        auto it = std::find_if(postings.begin(), postings.end(),
            [&doc_id](const Posting& p) { return p.doc_id == doc_id; });

        if (it != postings.end()) {
            it->term_frequency += freq;
            // Use max field weight (title > description)
            it->field_weight = std::max(it->field_weight, field_weight);
        } else {
            postings.push_back({doc_id, freq, field_weight});
        }
    }
    (void)field;  // field name used conceptually; weight carries the information
}

void InvertedIndex::remove_document(const std::string& doc_id) {
    auto it = doc_terms_.find(doc_id);
    if (it == doc_terms_.end()) return;

    // Remove from all posting lists
    for (const auto& term : it->second) {
        auto& postings = index_[term];
        postings.erase(
            std::remove_if(postings.begin(), postings.end(),
                [&doc_id](const Posting& p) { return p.doc_id == doc_id; }),
            postings.end()
        );

        // Remove empty posting lists
        if (postings.empty()) {
            index_.erase(term);
        }
    }

    doc_terms_.erase(it);
    doc_lengths_.erase(doc_id);
    doc_count_--;
}

std::vector<InvertedIndex::Posting> InvertedIndex::search_term(
    const std::string& term
) const {
    auto it = index_.find(term);
    if (it == index_.end()) return {};
    return it->second;
}

std::vector<std::string> InvertedIndex::search_and(
    const std::vector<std::string>& terms
) const {
    if (terms.empty()) return {};

    // Start with the posting list of the rarest term (smallest list)
    std::vector<std::pair<std::string, size_t>> term_sizes;
    for (const auto& term : terms) {
        auto it = index_.find(term);
        if (it == index_.end()) return {};  // If any term missing, AND = empty
        term_sizes.emplace_back(term, it->second.size());
    }

    // Sort by posting list size to optimize intersection
    std::sort(term_sizes.begin(), term_sizes.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    // Start with smallest posting list
    std::unordered_set<std::string> candidates;
    const auto& first_postings = index_.at(term_sizes[0].first);
    for (const auto& p : first_postings) {
        candidates.insert(p.doc_id);
    }

    // Intersect with remaining posting lists
    for (size_t i = 1; i < term_sizes.size(); i++) {
        const auto& postings = index_.at(term_sizes[i].first);
        std::unordered_set<std::string> posting_set;
        for (const auto& p : postings) {
            posting_set.insert(p.doc_id);
        }

        std::unordered_set<std::string> intersection;
        for (const auto& c : candidates) {
            if (posting_set.count(c)) {
                intersection.insert(c);
            }
        }
        candidates = std::move(intersection);

        if (candidates.empty()) return {};
    }

    return {candidates.begin(), candidates.end()};
}

std::vector<std::string> InvertedIndex::search_or(
    const std::vector<std::string>& terms
) const {
    std::unordered_set<std::string> result_set;
    for (const auto& term : terms) {
        auto it = index_.find(term);
        if (it != index_.end()) {
            for (const auto& p : it->second) {
                result_set.insert(p.doc_id);
            }
        }
    }
    return {result_set.begin(), result_set.end()};
}

double InvertedIndex::tf_idf(
    const std::string& term,
    const std::string& doc_id
) const {
    auto it = index_.find(term);
    if (it == index_.end()) return 0.0;

    // Find the posting for this document
    const auto& postings = it->second;
    auto pit = std::find_if(postings.begin(), postings.end(),
        [&doc_id](const Posting& p) { return p.doc_id == doc_id; });

    if (pit == postings.end()) return 0.0;

    // TF: augmented term frequency (prevents bias toward longer docs)
    double doc_len = static_cast<double>(doc_lengths_.count(doc_id) 
        ? doc_lengths_.at(doc_id) : 1);
    double tf = static_cast<double>(pit->term_frequency) / doc_len;

    // IDF: inverse document frequency with smoothing
    double df = static_cast<double>(postings.size());
    double n = static_cast<double>(doc_count_);
    double idf = std::log((n + 1.0) / (df + 1.0)) + 1.0;

    // Apply field weight boost
    return tf * idf * pit->field_weight;
}

std::unordered_map<std::string, double> InvertedIndex::search_scored(
    const std::vector<std::string>& terms
) const {
    std::unordered_map<std::string, double> scores;

    for (const auto& term : terms) {
        auto it = index_.find(term);
        if (it == index_.end()) continue;

        for (const auto& posting : it->second) {
            scores[posting.doc_id] += tf_idf(term, posting.doc_id);
        }
    }

    return scores;
}

int InvertedIndex::document_frequency(const std::string& term) const {
    auto it = index_.find(term);
    return it == index_.end() ? 0 : static_cast<int>(it->second.size());
}

size_t InvertedIndex::document_count() const {
    return doc_count_;
}

size_t InvertedIndex::term_count() const {
    return index_.size();
}

void InvertedIndex::clear() {
    index_.clear();
    doc_terms_.clear();
    doc_lengths_.clear();
    doc_count_ = 0;
}

bool InvertedIndex::contains_term(const std::string& term) const {
    return index_.count(term) > 0;
}

} // namespace algosearch
