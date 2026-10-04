#pragma once

#include "search_engine.hpp"
#include <string>
#include <memory>

namespace algosearch {

/**
 * HTTP server wrapper for the search engine.
 * 
 * Exposes search, autocomplete, and recommendation functionality
 * over a JSON REST API using cpp-httplib.
 * 
 * Endpoints:
 *   GET  /health                     - Health check
 *   POST /api/load                   - Load problem data
 *   GET  /api/search?q=...&limit=... - Search
 *   GET  /api/autocomplete?q=...     - Autocomplete
 *   GET  /api/problems/:id           - Get problem
 *   GET  /api/problems/:id/related   - Related problems
 *   GET  /api/problems               - List all problems
 *   GET  /api/topics                 - List topics
 *   GET  /api/algorithms             - List algorithms
 *   GET  /api/topics/:slug/problems  - Problems by topic
 *   GET  /api/stats                  - Engine statistics
 */
class HttpServer {
public:
    explicit HttpServer(int port = 8080);
    ~HttpServer();

    // Start the server (blocking)
    void start();

    // Stop the server
    void stop();

    // Get reference to the engine for direct manipulation
    SearchEngine& engine();

private:
    int port_;
    SearchEngine engine_;

    struct Impl;
    std::unique_ptr<Impl> impl_;

    void setup_routes();
};

} // namespace algosearch
