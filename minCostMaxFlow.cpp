#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

const int INF = 1e9;

struct Edge{
    int from;
    int to;
    int flow;
    int capacity;
    int cost;
    size_t id;
};

class Graph{
 public:
    Graph() {}

    Graph(int n) {
        links.resize(n);
    }

    void addEdge(int from, int to, int capacity, int cost) {
        links[from].push_back(edges.size());
        edges.push_back({from, to, 0, capacity, cost, edges.size()});
        links[to].push_back(edges.size());
        edges.push_back({to, from, 0, 0, -cost, edges.size()});
    }

    size_t size() const {
        return links.size();
    }

    size_t fromSize(int from) const {
        return links[from].size();
    }

    const Edge& edgeFrom(int from, int ind) const {
        return edges[links[from][ind]];
    }

    const Edge& revEdgeFrom(int from, int ind) const {
        return edges[getRevId(links[from][ind])];
    }

    const Edge& getEdge(int id) const {
        return edges[id];
    }

    int getRevId(int id) const {
        return id ^ 1;
    }

    void addFlow(int id, int flow) {
        edges[id].flow += flow;
        edges[getRevId(id)].flow -= flow;
    }

 private:
    std::vector<std::vector<int>> links;
    std::vector<Edge> edges;
};

class PDistance{
 public:
    PDistance() {}

    PDistance(int n, int s = 0, int val = INF) {
        dist.resize(n, val);
        from.resize(n, -1);
        dist[s] = 0;
    }

    void clear(int val = INF) {
        dist.assign(dist.size(), val);
        from.assign(from.size(), -1);
    }

    void reinit(int s = 0, int val = INF) {
        clear(val);
        dist[s] = 0;
    }

    void set(int v, int d, int by) {
        dist[v] = d;
        from[v] = by;
    }

    int get(int v) const {
        return dist[v];
    }

    std::vector<int> getPathFrom(int t, Graph& graph) const {
        std::vector<int> path;

        while (from[t] != -1) {
            path.push_back(from[t]);
            t = graph.getEdge(from[t]).from;
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

 private:
    std::vector<int> dist;
    std::vector<int> from;
};

int calcCost(int v, int ind, const Graph& graph, const PDistance& potentials) {
    const Edge& e = graph.edgeFrom(v, ind);
    return (e.capacity - e.flow) * (e.cost + potentials.get(e.from) - potentials.get(e.to));
}

PDistance Dijkstra(int s, Graph& graph, const PDistance& potentials) {
    PDistance dist(graph.size(), s);
    std::set<std::pair<int, int>> q;

    q.insert({0, s});

    while (!q.empty()) {
        int now = q.begin()->second;
        q.erase(q.begin());

        for (size_t i = 0; i < graph.fromSize(now); ++i) {
            const Edge& e = graph.edgeFrom(now, i);

            if (e.flow < e.capacity && dist.get(now) + calcCost(now, i, graph, potentials)
                    < dist.get(e.to)) {
                q.erase({dist.get(e.to), e.to});
                dist.set(e.to, dist.get(now) + calcCost(now, i, graph, potentials), e.id);
                q.insert({dist.get(e.to), e.to});
            }
        }
    }

    return dist;
}

int findMinCostKFlow(int k, int s, int t, Graph& graph) {
    PDistance potentials(graph.size(), 0, 0);
    int cost = 0;

    for (int flow = 0; flow < k;) {
        PDistance dist = Dijkstra(s, graph, potentials);
        if (dist.get(t) == INF) return -1;

        cost += dist.get(t) - potentials.get(s) + potentials.get(t);
        ++flow;
        std::vector<int> path = dist.getPathFrom(t, graph);

        for (auto edgeId : path) {
            graph.addFlow(edgeId, 1);
        }

        potentials = dist;
    }

    return cost;
}

bool findPath(int v, int t, Graph& graph, std::vector<bool>& was, std::vector<int>& path) {
    was[v] = true;

    if (v == t) return true;

    for (size_t i = 0; i < graph.fromSize(v); ++i) {
        const Edge& e = graph.edgeFrom(v, i);

        if (!was[e.to] && e.flow > 0) {
            path.push_back(e.id);
            if (findPath(e.to, t, graph, was, path)) {
                graph.addFlow(e.id, -1);
                return true;
            }
            path.pop_back();
        }
    }

    return false;
}

int main() {
    std::ios::sync_with_stdio(false);

    int n, m, k;
    std::cin >> n >> m >> k;

    Graph graph(n);

    for (int i = 0; i < m; ++i) {
        int from, to, cost;
        std::cin >> from >> to >> cost;
        --from, --to;

        graph.addEdge(from, to, 1, cost);
        graph.addEdge(to, from, 1, cost);
    }

    int cost = findMinCostKFlow(k, 0, n - 1, graph);

    if (cost == -1) {
        std::cout << "-1\n";
        return 0;
    }

    std::cout << std::fixed << std::setprecision(6);
    std::cout << static_cast<double>(cost) / k << "\n";

    for (int i = 0; i < k; ++i) {
        std::vector<bool> was(n);
        std::vector<int> path;

        findPath(0, n - 1, graph, was, path);

        std::cout << path.size() << " ";
        for (auto id : path) {
            std::cout << id / 4 + 1 << " ";
        }
        std::cout << "\n";
    }

    return 0;
}

