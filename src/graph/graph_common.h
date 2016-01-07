#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "debug_msg_handler.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/graph/undirected_dfs.hpp>

#ifndef GRAPH_COMMON_H
#define GRAPH_COMMON_H

enum Layer{
    FUNCTION,
    TASK,
    CONTROLLER,
    RESOURCE,
    PHYSICAL,
    LAYER_ERROR
};

enum Priority{
    NO_PRIORITY,
    MISSION_CRITICAL,
    SAFETY_CRITICAL,
    PRIORITY_ENUM_SIZE
};

enum Component_Priority_Category{
    ROUND_ROBIN,
    PRIORITY,
    TDMA,
    PRIORITY_CATEGORY_ERROR
};

enum Component_Type{
  PROCESSOR,
  BUS,
  BRIDGE,
  PERIPHERAL,
  MEMORY,
  TYPE_ERROR
};





typedef struct{
  std::string component_name;
  int component_port;
}Edge_Port;

class Custom_Vertex
{
public:
    Layer layer;
    std::string name;
    std::map<int,int> ports; //TO REFINE tipo aggiungere solo per Lay 4.
    Component_Type type;
    Component_Priority_Category priority_category;
};

class Custom_Edge
{
public:
    Custom_Edge(void) : priority(NO_PRIORITY){}
    Custom_Edge(Priority p) : priority(p){}
    Priority priority;
    Edge_Port to_port;
    Edge_Port from_port;
};


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,Custom_Vertex,Custom_Edge>Source_Graph;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,Custom_Vertex,Custom_Edge> Timing_Graph;
typedef boost::graph_traits<Source_Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Source_Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Timing_Graph>::edge_descriptor inner_edge_t;
typedef boost::graph_traits<Source_Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Timing_Graph>::vertex_iterator inner_vertex_iter;
typedef boost::graph_traits<Source_Graph>::edge_iterator edge_iter;
typedef boost::graph_traits<Timing_Graph>::edge_iterator inner_edge_iter;
typedef boost::graph_traits<Source_Graph>::out_edge_iterator ext_out_edge_iter;

template <class Layer_Map,class Name_Map,class Ports_Map, class Type_Map, class Priority_Type_Map>
class vertex_writer {
public:
  vertex_writer(Layer_Map l, Name_Map n, Ports_Map p_m, Type_Map t, Priority_Type_Map p) : lm(l),nm(n),pm(p_m), tm(t),ptm(p) {}
  template <class Vertex>
  void operator()(std::ostream &out, const Vertex& v) const {
    PRINT_DEBUG("printing graph: node is: "+nm[v]+" and its type is: "+boost::lexical_cast<std::string>(tm[v])+" and its priority type is: "+boost::lexical_cast<std::string>(ptm[v]));
    out << "[label=< <FONT POINT-SIZE=\"20\">"<<nm[v]<<"</FONT><br/>"<<tm[v]<<"<br/><FONT POINT-SIZE=\"10\">"<<ptm[v]<<"</FONT> >";
    switch(lm[v]){
      case FUNCTION:
      {
	out<<",style=filled, fillcolor= yellow";
	break;
      }
      case TASK:
      {
	out<<",style=filled, fillcolor= red";
	break;
      }
      case CONTROLLER:
      {
	out<<",style=filled, fillcolor= green";
	break;
      }
      case RESOURCE:
      {
	out<<",style=filled, fillcolor= cyan";
	break;
      }
      case PHYSICAL:
      {break;}
      default:
      {
	PRINT_DEBUG("ERROR: lm[e] not a layer type ");
	throw std::runtime_error(boost::lexical_cast<std::string>("ERROR: lm[e] not a layer "));
      }
    }
    out << "]";
    //ports missing
  }
private:
  Layer_Map lm;
  Name_Map nm;
  Ports_Map pm;
  Type_Map tm;
  Priority_Type_Map ptm;
};

template <class Layer_Map,class Name_Map,class Ports_Map, class Type_Map, class Priority_Type_Map>
inline vertex_writer<Layer_Map, Name_Map, Ports_Map,  Type_Map,  Priority_Type_Map> 
make_vertex_writer(Layer_Map l,Name_Map n,Ports_Map p, Type_Map t, Priority_Type_Map pt) {
  return vertex_writer<Layer_Map, Name_Map, Ports_Map, Type_Map, Priority_Type_Map>(l,n,p,t,pt);
}


template <class Priority_Map,class From_Port_Map,class To_Port_Map>
class edge_writer {
public:
  edge_writer(Priority_Map l, From_Port_Map n, To_Port_Map p_m) : pm(l),fpm(n),tpm(p_m) {}
  template <class Edge>
  void operator()(std::ostream &out, const Edge& e) const {
    PRINT_DEBUG("printing graph: edge is ("+boost::lexical_cast<std::string>(fpm[e])+","+boost::lexical_cast<std::string>(tpm[e])+") and its priority is: "+boost::lexical_cast<std::string>(pm[e]));
    out <<  "[label=\"" << pm[e] << "\", taillabel=\"" << tpm[e] << "\", headlabel=\""<<fpm[e]<<"\"]";
  }
private:
  Priority_Map pm;
  From_Port_Map fpm;
  To_Port_Map tpm;
};

template <class Priority_Map,class From_Port_Map,class To_Port_Map>
inline edge_writer<Priority_Map, From_Port_Map, To_Port_Map> 
make_edge_writer(Priority_Map l, From_Port_Map n, To_Port_Map p) {
  return edge_writer<Priority_Map, From_Port_Map, To_Port_Map>(l,n,p);
}

#endif
