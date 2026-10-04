#include "algosearch/search_engine.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void print_help() {
    std::cout << R"(
AlgoSearch CLI - Test the search engine interactively

Usage:
  algosearch_cli <data_file> [command] [args...]

Commands:
  search <query>        Search for problems
  autocomplete <prefix> Get autocomplete suggestions
  related <problem_id>  Find related problems
  topics                List all topics
  algorithms            List all algorithms
  stats                 Show engine statistics
  interactive           Interactive mode (default)

Example:
  algosearch_cli data/seed.json search "dijkstra shortest path"
  algosearch_cli data/seed.json autocomplete "gra"
  algosearch_cli data/seed.json interactive
)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    std::string data_file = argv[1];

    // Load data
    std::ifstream f(data_file);
    if (!f.is_open()) {
        std::cerr << "Error: Cannot open file: " << data_file << std::endl;
        return 1;
    }

    json data = json::parse(f);
    std::vector<algosearch::Problem> problems;

    if (data.contains("problems")) {
        problems = data["problems"].get<std::vector<algosearch::Problem>>();
    } else if (data.is_array()) {
        problems = data.get<std::vector<algosearch::Problem>>();
    }

    algosearch::SearchEngine engine;
    algosearch::Timer timer;
    engine.load_problems(problems);
    std::cout << "Loaded " << problems.size() << " problems in "
              << timer.elapsed_ms() << "ms\n" << std::endl;

    // Get command
    std::string command = argc > 2 ? argv[2] : "interactive";

    if (command == "search" && argc > 3) {
        std::string query;
        for (int i = 3; i < argc; i++) {
            if (i > 3) query += " ";
            query += argv[i];
        }

        algosearch::SearchQuery sq;
        sq.query = query;
        sq.limit = 10;

        auto result = engine.search(sq);
        std::cout << "Search: \"" << query << "\"\n";
        std::cout << "Found: " << result.total_count << " results in "
                  << result.search_duration_ms << "ms\n\n";

        for (const auto& r : result.results) {
            std::cout << "  [" << r.problem.id << "] " << r.problem.title
                      << " (" << r.problem.difficulty << ")"
                      << " - Score: " << r.score << "\n";
        }
    }
    else if (command == "autocomplete" && argc > 3) {
        std::string prefix = argv[3];
        auto suggestions = engine.autocomplete(prefix);
        std::cout << "Autocomplete for \"" << prefix << "\":\n";
        for (const auto& s : suggestions) {
            std::cout << "  " << s.term << " [" << s.category
                      << "] (freq: " << s.frequency << ")\n";
        }
    }
    else if (command == "related" && argc > 3) {
        std::string id = argv[3];
        auto related = engine.get_related_problems(id);
        std::cout << "Related to " << id << ":\n";
        for (const auto& r : related) {
            std::cout << "  [" << r.problem.id << "] " << r.problem.title
                      << " - Similarity: " << r.similarity << "\n";
            if (!r.shared_topics.empty()) {
                std::cout << "    Shared: ";
                for (const auto& t : r.shared_topics) std::cout << t << " ";
                std::cout << "\n";
            }
        }
    }
    else if (command == "topics") {
        auto topics = engine.get_topics();
        std::cout << "Topics (" << topics.size() << "):\n";
        for (const auto& t : topics) {
            auto probs = engine.get_problems_by_topic(t);
            std::cout << "  " << t << " (" << probs.size() << " problems)\n";
        }
    }
    else if (command == "algorithms") {
        auto algos = engine.get_algorithms();
        std::cout << "Algorithms (" << algos.size() << "):\n";
        for (const auto& a : algos) {
            auto probs = engine.get_problems_by_algorithm(a);
            std::cout << "  " << a << " (" << probs.size() << " problems)\n";
        }
    }
    else if (command == "stats") {
        auto stats = engine.get_stats();
        std::cout << "Engine Statistics:\n";
        std::cout << "  Problems: " << stats.total_problems << "\n";
        std::cout << "  Index terms: " << stats.index_terms << "\n";
        std::cout << "  Trie terms: " << stats.trie_terms << "\n";
        std::cout << "  Graph nodes: " << stats.graph_nodes << "\n";
        std::cout << "  Topics: " << stats.topics_count << "\n";
        std::cout << "  Algorithms: " << stats.algorithms_count << "\n";
    }
    else {
        // Interactive mode
        std::cout << "AlgoSearch Interactive CLI\n";
        std::cout << "Commands: search, autocomplete, related, topics, algorithms, stats, quit\n\n";

        std::string line;
        while (true) {
            std::cout << "algosearch> ";
            if (!std::getline(std::cin, line)) break;
            if (line.empty()) continue;
            if (line == "quit" || line == "exit") break;

            auto tokens = algosearch::tokenize(line);
            if (tokens.empty()) continue;

            if (tokens[0] == "search" && line.size() > 7) {
                std::string q = line.substr(7);
                algosearch::SearchQuery sq;
                sq.query = q;
                sq.limit = 10;
                auto result = engine.search(sq);
                std::cout << "Found " << result.total_count << " results in "
                          << result.search_duration_ms << "ms\n";
                for (const auto& r : result.results) {
                    std::cout << "  [" << r.problem.id << "] " << r.problem.title
                              << " (" << r.problem.difficulty << ")"
                              << " Score: " << r.score << "\n";
                }
            }
            else if (tokens[0] == "autocomplete" && line.size() > 13) {
                std::string prefix = line.substr(13);
                auto suggestions = engine.autocomplete(prefix);
                for (const auto& s : suggestions) {
                    std::cout << "  " << s.term << " [" << s.category << "]\n";
                }
            }
            else if (tokens[0] == "stats") {
                auto stats = engine.get_stats();
                std::cout << "  Problems: " << stats.total_problems
                          << "  Terms: " << stats.index_terms
                          << "  Trie: " << stats.trie_terms << "\n";
            }
            else {
                std::cout << "Unknown command. Try: search, autocomplete, stats, quit\n";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}
