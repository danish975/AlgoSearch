#include "algosearch/http_server.hpp"

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

namespace algosearch {

struct HttpServer::Impl {
    httplib::Server server;
};

HttpServer::HttpServer(int port) : port_(port), impl_(std::make_unique<Impl>()) {
    setup_routes();
}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::start() {
    std::cout << "[AlgoSearch Engine] Starting HTTP server on port " << port_ << std::endl;
    impl_->server.listen("0.0.0.0", port_);
}

void HttpServer::stop() {
    impl_->server.stop();
}

SearchEngine& HttpServer::engine() {
    return engine_;
}

void HttpServer::setup_routes() {
    auto& svr = impl_->server;

    // ── CORS headers ───────────────────────────────────────────────
    svr.set_pre_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return httplib::Server::HandlerResponse::Unhandled;
    });

    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
    });

    // ── Health check ───────────────────────────────────────────────
    svr.Get("/health", [this](const httplib::Request&, httplib::Response& res) {
        json response = {
            {"status", "healthy"},
            {"service", "algosearch-engine"},
            {"problems_loaded", engine_.problem_count()}
        };
        res.set_content(response.dump(), "application/json");
    });

    // ── Load problems ──────────────────────────────────────────────
    svr.Post("/api/load", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto data = json::parse(req.body);
            std::vector<Problem> problems;

            if (data.contains("problems") && data["problems"].is_array()) {
                problems = data["problems"].get<std::vector<Problem>>();
            } else if (data.is_array()) {
                problems = data.get<std::vector<Problem>>();
            }

            Timer timer;
            engine_.load_problems(problems);

            json response = {
                {"success", true},
                {"message", "Loaded " + std::to_string(problems.size()) + " problems"},
                {"count", problems.size()},
                {"index_time_ms", timer.elapsed_ms()}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {
                {"success", false},
                {"error", std::string("Failed to load problems: ") + e.what()}
            };
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // ── Search ─────────────────────────────────────────────────────
    svr.Get("/api/search", [this](const httplib::Request& req, httplib::Response& res) {
        SearchQuery query;
        query.query = req.get_param_value("q");
        if (req.has_param("limit")) query.limit = std::stoi(req.get_param_value("limit"));
        if (req.has_param("offset")) query.offset = std::stoi(req.get_param_value("offset"));
        if (req.has_param("difficulty")) query.difficulty = req.get_param_value("difficulty");
        if (req.has_param("topic")) query.topic = req.get_param_value("topic");
        if (req.has_param("algorithm")) query.algorithm = req.get_param_value("algorithm");
        if (req.has_param("platform")) query.platform = req.get_param_value("platform");
        if (req.has_param("sort_by")) query.sort_by = req.get_param_value("sort_by");

        auto result = engine_.search(query);

        // Track search for popularity
        if (!query.query.empty()) {
            engine_.track_search(query.query);
        }

        json response = result;
        res.set_content(response.dump(), "application/json");
    });

    // ── Autocomplete ───────────────────────────────────────────────
    svr.Get("/api/autocomplete", [this](const httplib::Request& req, httplib::Response& res) {
        std::string q = req.get_param_value("q");
        int limit = 8;
        if (req.has_param("limit")) limit = std::stoi(req.get_param_value("limit"));

        auto suggestions = engine_.autocomplete(q, limit);

        json response = {
            {"suggestions", suggestions},
            {"query", q}
        };
        res.set_content(response.dump(), "application/json");
    });

    // ── Get all problems ───────────────────────────────────────────
    svr.Get("/api/problems", [this](const httplib::Request& req, httplib::Response& res) {
        int limit = 20;
        int offset = 0;
        if (req.has_param("limit")) limit = std::stoi(req.get_param_value("limit"));
        if (req.has_param("offset")) offset = std::stoi(req.get_param_value("offset"));

        auto all = engine_.get_all_problems();

        int total = static_cast<int>(all.size());
        int start = std::min(offset, total);
        int end = std::min(start + limit, total);

        std::vector<Problem> page(all.begin() + start, all.begin() + end);

        json response = {
            {"problems", page},
            {"total", total},
            {"limit", limit},
            {"offset", offset}
        };
        res.set_content(response.dump(), "application/json");
    });

    // ── Get problem by ID ──────────────────────────────────────────
    svr.Get(R"(/api/problems/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        auto* problem = engine_.get_problem(id);

        if (!problem) {
            json error = {{"error", "Problem not found"}, {"id", id}};
            res.status = 404;
            res.set_content(error.dump(), "application/json");
            return;
        }

        json response = *problem;
        res.set_content(response.dump(), "application/json");
    });

    // ── Related problems ───────────────────────────────────────────
    svr.Get(R"(/api/problems/([^/]+)/related)", [this](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        int limit = 10;
        if (req.has_param("limit")) limit = std::stoi(req.get_param_value("limit"));

        auto related = engine_.get_related_problems(id, limit);

        json response = {
            {"related", related},
            {"problem_id", id}
        };
        res.set_content(response.dump(), "application/json");
    });

    // ── Topics ─────────────────────────────────────────────────────
    svr.Get("/api/topics", [this](const httplib::Request&, httplib::Response& res) {
        auto topics = engine_.get_topics();

        json topic_list = json::array();
        for (const auto& t : topics) {
            auto problems = engine_.get_problems_by_topic(t);
            topic_list.push_back({
                {"name", t},
                {"slug", t},
                {"problem_count", problems.size()}
            });
        }

        json response = {{"topics", topic_list}};
        res.set_content(response.dump(), "application/json");
    });

    // ── Problems by topic ──────────────────────────────────────────
    svr.Get(R"(/api/topics/([^/]+)/problems)", [this](const httplib::Request& req, httplib::Response& res) {
        std::string slug = req.matches[1];
        auto problems = engine_.get_problems_by_topic(slug);

        json response = {
            {"topic", slug},
            {"problems", problems},
            {"total", problems.size()}
        };
        res.set_content(response.dump(), "application/json");
    });

    // ── Algorithms ─────────────────────────────────────────────────
    svr.Get("/api/algorithms", [this](const httplib::Request&, httplib::Response& res) {
        auto algorithms = engine_.get_algorithms();

        json algo_list = json::array();
        for (const auto& a : algorithms) {
            auto problems = engine_.get_problems_by_algorithm(a);
            algo_list.push_back({
                {"name", a},
                {"slug", a},
                {"problem_count", problems.size()}
            });
        }

        json response = {{"algorithms", algo_list}};
        res.set_content(response.dump(), "application/json");
    });

    // ── Stats ──────────────────────────────────────────────────────
    svr.Get("/api/stats", [this](const httplib::Request&, httplib::Response& res) {
        auto stats = engine_.get_stats();

        json response = {
            {"total_problems", stats.total_problems},
            {"index_terms", stats.index_terms},
            {"trie_terms", stats.trie_terms},
            {"graph_nodes", stats.graph_nodes},
            {"topics_count", stats.topics_count},
            {"algorithms_count", stats.algorithms_count}
        };
        res.set_content(response.dump(), "application/json");
    });

    std::cout << "[AlgoSearch Engine] Routes configured" << std::endl;
}

} // namespace algosearch
