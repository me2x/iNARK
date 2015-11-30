#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>

typedef enum{
    FUNCTION,
    TASK,
    CONTROLLER,
    RESOURCE,
    PHYSICAL
}Layer;

typedef enum{
    NO_PRIORITY,
    MISSION_CRITICAL,
    SAFETY_CRITICAL
}Priority;

class Custom_Vertex
{
public:
    Layer layer;
    std::string name;
};
class Custom_Edge
{
public:
    Custom_Edge(void) : priority(NO_PRIORITY){}
    Custom_Edge(Priority p) : priority(p){}
    Priority priority;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,Custom_Vertex,Custom_Edge> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;


class custom_edge_predicate_c {
public:
  custom_edge_predicate_c() : graph_m(0) ,threshold(NO_PRIORITY){}
  custom_edge_predicate_c(Graph& graph,Priority threshold) : graph_m(&graph), threshold (threshold) {}//non testata.
  bool operator()(const edge_t& edge_id) const {
    Priority type = (*graph_m)[edge_id].priority;
    return (type >= threshold);
  }
private:
  Graph* graph_m;
  Priority threshold;
};


class my_visitor :public boost::default_bfs_visitor{
protected:
  vertex_t destination_vertex_m;
public:
  my_visitor(vertex_t destination_vertex_l): destination_vertex_m(destination_vertex_l) {
      std::cout<<"blah"<<std::endl;
      std::cout<<destination_vertex_m<<std::endl;
  }
  void initialize_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
  void discover_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g)const {
      std::cout<<"exploring, found vertex: "<<s<<std::endl;

      if (destination_vertex_m == s)
        throw(2);
  }
  void examine_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
  void examine_edge(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
  void edge_relaxed(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
  void edge_not_relaxed(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
  void finish_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
};

class custom_graph {
public:
    custom_graph() {}

    bool filter_and_explore_graph(Priority p,std::string from, std::string to);
    bool create_graph(std::string xml_location);


private:
    Graph local_graph;
    std::map<std::string, vertex_t> vertex_map;
};
