#include "algosearch/http_server.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static algosearch::HttpServer* g_server = nullptr;

void signal_handler(int sig) {
    std::cout << "\n[AlgoSearch Engine] Received signal " << sig << ", shutting down..." << std::endl;
    if (g_server) {
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    int port = 8080;

    // Parse port from env or args
    if (const char* env_port = std::getenv("SEARCH_ENGINE_PORT")) {
        port = std::atoi(env_port);
    }
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    algosearch::HttpServer server(port);
    g_server = &server;

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Load seed data if file is provided
    std::string data_file;
    if (argc > 2) {
        data_file = argv[2];
    } else if (const char* env_data = std::getenv("SEED_DATA_FILE")) {
        data_file = env_data;
    }

    if (!data_file.empty()) {
        std::cout << "[AlgoSearch Engine] Loading seed data from: " << data_file << std::endl;
        try {
            std::ifstream f(data_file);
            if (f.is_open()) {
                json data = json::parse(f);
                std::vector<algosearch::Problem> problems;

                if (data.contains("problems")) {
                    problems = data["problems"].get<std::vector<algosearch::Problem>>();
                } else if (data.is_array()) {
                    problems = data.get<std::vector<algosearch::Problem>>();
                }

                algosearch::Timer timer;
                server.engine().load_problems(problems);
                std::cout << "[AlgoSearch Engine] Loaded " << problems.size()
                          << " problems in " << timer.elapsed_ms() << "ms" << std::endl;
            } else {
                std::cerr << "[AlgoSearch Engine] Warning: Could not open data file: " << data_file << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[AlgoSearch Engine] Error loading seed data: " << e.what() << std::endl;
        }
    }

    std::cout << "[AlgoSearch Engine] Server starting on http://0.0.0.0:" << port << std::endl;
    server.start();

    return 0;
}
