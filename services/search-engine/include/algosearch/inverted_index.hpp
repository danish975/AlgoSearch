#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

namespace algosearch {

/**
 * Inverted Index for full-text search retrieval.
 * 
 * Maps terms to posting lists containing document IDs and term frequencies.
 * Supports TF-IDF scoring, boolean AND/OR retrieval, and field-weighted search.
 * 
 * Complexity:
 *   Index a document: O(T) where T = number of tokens
 *   Search (single term): O(1) lookup + O(P) where P = posting list size
 *   Boolean AND: O(min(P1, P2, ...)) with sorted intersection
 *   Boolean OR: O(P1 + P2 + ...)
 */
class InvertedIndex {
public:
    // Posting entry: document ID + term frequency in that document
    struct Posting {
        std::string doc_id;
        int term_frequency = 0;
        double field_weight = 1.0;  // higher for title matches vs description
    };

    InvertedIndex();
    ~InvertedIndex();

    // Index a document with field-weighted tokens
    // field = "title" gets higher weight than "description"
    void add_document(
        const std::string& doc_id,
        const std::string& field,
        const std::vector<std::string>& tokens,
        double field_weight = 1.0
    );

    // Remove a document from the index
    void remove_document(const std::string& doc_id);

    // Search for a single term, returns posting list
    std::vector<Posting> search_term(const std::string& term) const;

    // Boolean AND: documents containing ALL query terms
    std::vector<std::string> search_and(const std::vector<std::string>& terms) const;

    // Boolean OR: documents containing ANY query terms
    std::vector<std::string> search_or(const std::vector<std::string>& terms) const;

    // Calculate TF-IDF score for a term in a document
    double tf_idf(const std::string& term, const std::string& doc_id) const;

    // Get scored results for a multi-term query
    // Returns map of doc_id -> cumulative TF-IDF score
    std::unordered_map<std::string, double> search_scored(
        const std::vector<std::string>& terms
    ) const;

    // Get document frequency (number of documents containing term)
    int document_frequency(const std::string& term) const;

    // Get total number of indexed documents
    size_t document_count() const;

    // Get total number of unique terms
    size_t term_count() const;

    // Clear the entire index
    void clear();

    // Check if index contains a term
    bool contains_term(const std::string& term) const;

private:
    // term -> list of postings
    std::unordered_map<std::string, std::vector<Posting>> index_;

    // doc_id -> set of terms (for removal)
    std::unordered_map<std::string, std::unordered_set<std::string>> doc_terms_;

    // Total number of documents
    size_t doc_count_ = 0;

    // doc_id -> total token count (for TF normalization)
    std::unordered_map<std::string, int> doc_lengths_;
};

} // namespace algosearch
