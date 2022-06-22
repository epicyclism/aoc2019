#include <iostream>
#include <numeric>
#include <map>
#include <chrono>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include "boost/graph/boyer_myrvold_planar_test.hpp"

using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
	boost::property<boost::vertex_color_t, boost::default_color_type>>;

size_t vertex_id_from_name(std::string const& nm)
{
	static std::map<std::string, size_t> nm_vertex;
	if (auto it = nm_vertex.find(nm); it != nm_vertex.end())
		return (*it).second;
	auto rv = nm_vertex.size();
	nm_vertex.insert(std::make_pair(std::string(nm), rv));
	return rv;
}

graph_t build_graph()
{
	graph_t rg;
	std::string ln;
	while (std::getline(std::cin, ln))
	{
		auto sep = ln.find(')');
		add_edge(vertex_id_from_name(ln.substr(0, sep)), vertex_id_from_name(ln.substr(sep+1)), rg);
	}
	return rg;
}

auto pt1(graph_t& g)
{
	std::vector<size_t> distances(boost::num_vertices(g));
	boost::breadth_first_search(g,
		vertex(vertex_id_from_name("COM"), g),
		boost::visitor(
			boost::make_bfs_visitor(
				boost::record_distances(
					boost::make_iterator_property_map(distances.begin(), get(boost::vertex_index, g)), boost::on_tree_edge()))));

	return std::accumulate(distances.begin(), distances.end(), size_t(0));
}

auto pt2(graph_t& g)
{
	std::vector<int> distances(num_vertices(g));
	boost::breadth_first_search(g,
		boost::vertex(vertex_id_from_name("YOU"), g),
		boost::visitor(
			boost::make_bfs_visitor(
				boost::record_distances(
					boost::make_iterator_property_map(distances.begin(), get(boost::vertex_index, g)), boost::on_tree_edge()))));
	return distances[vertex_id_from_name("SAN")] - 2;
}

int main()
{
	auto start = std::chrono::system_clock::now();

	auto g = build_graph();
	std::cout << "graph " << (boost::boyer_myrvold_planarity_test(g) ? "is" : "isn't") << " planar\n";
	std::cout << "part 1 = " << pt1(g) << "\n";
	std::cout << "part 2 = " << pt2(g) << "\n";

	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}