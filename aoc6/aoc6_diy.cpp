#include <iostream>
#include <numeric>
#include <vector>
#include <map>
#include <queue>
#include <chrono>

using vertex_id_t = size_t;
using edge_t = vertex_id_t;
using graph_t = std::vector<std::vector<edge_t>>;

vertex_id_t vertex_id_from_name(std::string const& nm)
{
	static std::map<std::string, vertex_id_t> nm_vertex;
	if (auto it = nm_vertex.find(nm); it != nm_vertex.end())
		return (*it).second;
	auto rv = nm_vertex.size();
	nm_vertex.insert(std::make_pair(std::string(nm), rv));
	return rv;
}

void add_edge(vertex_id_t from, vertex_id_t to, graph_t& g)
{
	if (g.size() < from + 1)
		g.resize(from + 1);
	g[from].push_back(to);
}

void add_edge_undirected(vertex_id_t from, vertex_id_t to, graph_t& g)
{
	add_edge(from, to, g);
	add_edge(to, from, g);
}

std::vector<int> bfs(vertex_id_t id_from, graph_t const& g)
{
	std::vector<int> d(g.size());
    std::vector<bool>   visited(g.size());
    std::queue<vertex_id_t> q;
    q.push(id_from);
	int dist{ 0 };
    while (!q.empty())
    {
        auto u = q.front(); q.pop();
        visited[u] = true;
		d[u] = dist;
        for (auto v :g[u])
        {
            if(!visited[v])
                q.push(v);
        }
		++dist;
    }
    return d;
}

graph_t build_graph()
{
	graph_t g;
	std::string ln;
	while (std::getline(std::cin, ln))
	{
		auto sep = ln.find(')');
		add_edge(vertex_id_from_name(ln.substr(0, sep)), vertex_id_from_name(ln.substr(sep + 1)), g);
	}
	return g;
}

auto pt1(graph_t const& g)
{
	auto distances = bfs(vertex_id_from_name("COM"), g);
	return std::accumulate(distances.begin(), distances.end(), size_t(0));
}

auto pt2(graph_t const& g)
{
	auto distances = bfs(vertex_id_from_name("YOU"), g);
	return distances[vertex_id_from_name("SAN")] - 2;
}

int main()
{
	auto start = std::chrono::system_clock::now();

	auto g = build_graph();
	std::cout << "part 1 = " << pt1(g) << "\n";
	std::cout << "part 2 = " << pt2(g) << "\n";

	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}