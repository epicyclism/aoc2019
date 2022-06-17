#include <iostream>
#include <numeric>
#include <map>
#include <chrono>

#include <boost/graph/visitors.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/property_map/property_map.hpp>

using graph_t = boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
	boost::property<boost::vertex_color_t, boost::default_color_type>>;

using vertex_t = graph_t::vertex_descriptor;

std::map<std::string, size_t> nm_vertex;
size_t vertex_from_name(std::string const& nm)
{
	if (auto it = nm_vertex.find(nm); it == nm_vertex.end())
	{
		auto rv = nm_vertex.size();
		nm_vertex.insert(std::make_pair(std::string(nm), rv));
		return rv;
	}
	else
		return (*it).second;
}

graph_t build_graph()
{
	graph_t rg;
	std::string ln;
	while (std::getline(std::cin, ln))
	{
		auto sep = ln.find(')');
		add_edge(vertex_from_name(ln.substr(0, sep)), vertex_from_name(ln.substr(sep+1)), rg);
	}
	return rg;
}

void pt1(graph_t& g)
{
	std::vector<size_t> d(boost::num_vertices(g));
	vertex_t s = vertex(vertex_from_name("COM"), g);
	boost::breadth_first_search(g,
		s,
		boost::visitor(
			boost::make_bfs_visitor(
				boost::record_distances(
					boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g)), boost::on_tree_edge()))));

	std::cout << std::accumulate(d.begin(), d.end(), size_t(0)) << '\n';
}

void pt2(graph_t& g)
{
	std::vector<int> d(num_vertices(g));
	vertex_t s = vertex(vertex_from_name("YOU"), g);
	boost::breadth_first_search(g,
		s,
		boost::visitor(
			boost::make_bfs_visitor(
				boost::record_distances(
					boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g)), boost::on_tree_edge()))));
	std::cout << (d[vertex_from_name("SAN")] - 2) << '\n';
}

int main()
{
	auto start = std::chrono::system_clock::now();

	auto g = build_graph();
	pt1(g);
	pt2(g);

	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

}