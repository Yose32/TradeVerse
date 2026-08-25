#include <cmath>
#ifndef TRADEVERSE_GRAPH_HPP
#define TRADEVERSE_GRAPH_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <sstream>

namespace tradeverse {
namespace dsa {

/**
 * @brief Graph Step Log for DSA visualization and viva presentation.
 */
struct GraphStepLog {
    std::string algorithm;
    std::string currentNode;
    std::vector<std::string> visitedNodes;
    std::vector<std::string> frontier;
    std::unordered_map<std::string, double> distances;
    std::string description;
};

/**
 * @brief Custom Weighted Directed Graph implementation.
 * 
 * Used for:
 * - Market sector dependencies and supply-chain linkages
 * - Propagating shockwaves (e.g. semiconductor shortage, energy crises)
 * - Educational DSA Lab visualizer (BFS, DFS, Dijkstra)
 * 
 * Complexities:
 * - Graph representation: Adjacency list O(V + E)
 * - BFS: Time O(V + E), Space O(V)
 * - DFS: Time O(V + E), Space O(V)
 * - Dijkstra: Time O((V + E) log V), Space O(V)
 */
class Graph {
public:
    struct Edge {
        std::string target;
        double weight; // Correlation strength / propagation resistance (lower = faster propagation)
        std::string relationship; // e.g. "supplies", "sector_peer", "parent_company"
    };

private:
    std::unordered_map<std::string, std::vector<Edge>> adjList;
    std::vector<std::string> vertices;

public:
    Graph() = default;

    void addVertex(const std::string& vertex) {
        if (adjList.find(vertex) == adjList.end()) {
            adjList[vertex] = {};
            vertices.push_back(vertex);
        }
    }

    void addEdge(const std::string& source, const std::string& target, double weight, const std::string& rel = "correlated") {
        addVertex(source);
        addVertex(target);
        adjList[source].push_back({target, weight, rel});
    }

    void addBidirectionalEdge(const std::string& u, const std::string& v, double weight, const std::string& rel = "peer") {
        addEdge(u, v, weight, rel);
        addEdge(v, u, weight, rel);
    }

    const std::vector<std::string>& getVertices() const { return vertices; }
    const std::unordered_map<std::string, std::vector<Edge>>& getAdjList() const { return adjList; }

    std::vector<Edge> getNeighbors(const std::string& vertex) const {
        auto it = adjList.find(vertex);
        if (it != adjList.end()) {
            return it->second;
        }
        return {};
    }

    /**
     * @brief Breadth-First Search (BFS)
     * Traverses nodes level-by-level, ideal for unweighted contagion ripple effect.
     */
    std::vector<std::string> bfs(const std::string& startVertex, std::vector<GraphStepLog>* outLogs = nullptr) const {
        std::vector<std::string> result;
        if (adjList.find(startVertex) == adjList.end()) return result;

        std::unordered_map<std::string, bool> visited;
        std::queue<std::string> q;

        visited[startVertex] = true;
        q.push(startVertex);

        while (!q.empty()) {
            std::string curr = q.front();
            q.pop();
            result.push_back(curr);

            if (outLogs) {
                GraphStepLog log;
                log.algorithm = "BFS";
                log.currentNode = curr;
                log.visitedNodes = result;
                std::queue<std::string> qCopy = q;
                while (!qCopy.empty()) {
                    log.frontier.push_back(qCopy.front());
                    qCopy.pop();
                }
                log.description = "Visiting node " + curr + " and expanding unvisited neighbors.";
                outLogs->push_back(log);
            }

            auto neighbors = getNeighbors(curr);
            for (const auto& edge : neighbors) {
                if (!visited[edge.target]) {
                    visited[edge.target] = true;
                    q.push(edge.target);
                }
            }
        }
        return result;
    }

    /**
     * @brief Depth-First Search (DFS)
     * Traverses deeply along each branch, useful for deep supply chain dependency tracking.
     */
    std::vector<std::string> dfs(const std::string& startVertex, std::vector<GraphStepLog>* outLogs = nullptr) const {
        std::vector<std::string> result;
        if (adjList.find(startVertex) == adjList.end()) return result;

        std::unordered_map<std::string, bool> visited;
        dfsHelper(startVertex, visited, result, outLogs);
        return result;
    }

private:
    void dfsHelper(const std::string& curr, std::unordered_map<std::string, bool>& visited,
                   std::vector<std::string>& result, std::vector<GraphStepLog>* outLogs) const {
        visited[curr] = true;
        result.push_back(curr);

        if (outLogs) {
            GraphStepLog log;
            log.algorithm = "DFS";
            log.currentNode = curr;
            log.visitedNodes = result;
            log.description = "Recursively diving into node " + curr;
            outLogs->push_back(log);
        }

        auto neighbors = getNeighbors(curr);
        for (const auto& edge : neighbors) {
            if (!visited[edge.target]) {
                dfsHelper(edge.target, visited, result, outLogs);
            }
        }
    }

public:
    /**
     * @brief Dijkstra's Shortest Path Algorithm
     * Computes minimum correlation resistance path from source to all reachable nodes.
     */
    struct DijkstraResult {
        std::unordered_map<std::string, double> distances;
        std::unordered_map<std::string, std::string> predecessors;
        std::vector<GraphStepLog> logs;

        std::vector<std::string> getPathTo(const std::string& target) const {
            std::vector<std::string> path;
            auto it = distances.find(target);
            if (it == distances.end() || it->second == std::numeric_limits<double>::infinity()) {
                return path;
            }
            std::string curr = target;
            while (!curr.empty()) {
                path.push_back(curr);
                auto predIt = predecessors.find(curr);
                if (predIt == predecessors.end() || predIt->second == curr) break;
                curr = predIt->second;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
    };

    DijkstraResult dijkstra(const std::string& source) const {
        DijkstraResult res;
        for (const auto& v : vertices) {
            res.distances[v] = std::numeric_limits<double>::infinity();
        }
        res.distances[source] = 0.0;

        // Custom min-priority pair (distance, node)
        typedef std::pair<double, std::string> DistNode;
        std::priority_queue<DistNode, std::vector<DistNode>, std::greater<DistNode>> pq;
        pq.push({0.0, source});

        std::vector<std::string> visited;

        while (!pq.empty()) {
            auto topPair = pq.top();
            double d = topPair.first;
            std::string u = topPair.second;
            pq.pop();

            if (d > res.distances[u]) continue;
            visited.push_back(u);

            GraphStepLog log;
            log.algorithm = "Dijkstra";
            log.currentNode = u;
            log.visitedNodes = visited;
            log.distances = res.distances;
            std::ostringstream ss;
            ss << "Processing node " << u << " with min distance " << d;
            log.description = ss.str();
            res.logs.push_back(log);

            for (const auto& edge : getNeighbors(u)) {
                std::string v = edge.target;
                double weight = edge.weight;

                if (res.distances[u] + weight < res.distances[v]) {
                    res.distances[v] = res.distances[u] + weight;
                    res.predecessors[v] = u;
                    pq.push({res.distances[v], v});
                }
            }
        }

        return res;
    }

    /**
     * @brief Propagate market event shocks through the graph.
     * Computes shock impact multiplier for all related stocks.
     */
    std::unordered_map<std::string, double> propagateShock(const std::string& epicentre, double baseImpact) const {
        std::unordered_map<std::string, double> impacts;
        DijkstraResult dResult = dijkstra(epicentre);

        for (const auto& pair : dResult.distances) {
            const std::string& node = pair.first;
            double dist = pair.second;
            if (dist < std::numeric_limits<double>::infinity()) {
                // Impact decays exponentially with graph resistance distance
                double factor = std::exp(-0.8 * dist);
                impacts[node] = baseImpact * factor;
            }
        }
        return impacts;
    }
};

} // namespace dsa
} // namespace tradeverse

#endif // TRADEVERSE_GRAPH_HPP
