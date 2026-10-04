# AlgoSearch

An intelligent platform for discovering, searching, understanding, and practicing Data Structures and Algorithms problems.

## Architecture

AlgoSearch consists of three main components:
1. **Frontend**: Next.js App Router, Tailwind CSS, shadcn/ui.
2. **Backend**: Express.js API, MongoDB, Redis caching.
3. **Search Engine**: Custom C++ engine implementing Trie, Inverted Index, TF-IDF, Top-K Heap, Graph BFS/DFS/Dijkstra, and Levenshtein string matching.

## Prerequisites

- Docker and Docker Compose
- Node.js (for local development outside Docker)
- C++20 compatible compiler & CMake (if compiling engine locally)

## Running with Docker Compose (Recommended)

To spin up the entire stack including the C++ engine, MongoDB, Redis, API, and Frontend:

```bash
docker-compose up --build
```

The services will be available at:
- Web Frontend: `http://localhost:3000`
- API Backend: `http://localhost:4000`
- C++ Search Engine: `http://localhost:8080`

## Features

- **Semantic Search**: Fast search via C++ inverted index, with multi-signal ranking.
- **Real-time Autocomplete**: Trie-based prefix matching.
- **Graph Recommendations**: Topics, algorithms, and problems are linked in a graph.
- **Authentication**: JWT-based secure auth (via MongoDB).
- **Caching**: Redis caches search and autocomplete queries for high throughput.

## Project Structure

- `apps/web`: Next.js frontend
- `apps/api`: Express.js backend
- `services/search-engine`: C++ search engine source
- `data/seed.json`: Initial problem dataset

## Testing

For the C++ Engine (requires CMake + GTest):
```bash
cd services/search-engine
cmake -B build -S .
cmake --build build
./build/algosearch_tests
```
