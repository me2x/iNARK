#include "internal_graph.h"


internal_graph::internal_graph(const Graph& g){
    //collassa grafo

    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(g); vp.first != vp.second; ++vp.first)
    {
        vertex_t vt = boost::add_vertex(ig);
        ig[vt].layer=g[*vp.first].layer;
        ig[vt].name=g[*vp.first].name;
       /* if(*vp.first.ports){
            ig[vt].ports.insert(std::make_pair<int,Priority>(i_v.second.get_child("id").get_value<int>(),int_to_Priority(i_v.second.get_child("priority").get_value<int>())));
            }
        }
        if(v.second.get_child_optional("type")){
            local_graph[vt].type=int_To_Type(v.second.get_child("type").get_value<int>());
        }
        vertex_map.insert(std::make_pair(local_graph[vt].name, vt));*/
        PRINT_DEBUG(ig[vt].name);

    }

    //per ogni edge, crea andata e ritorno nel bidirezionale. tranne che per gli intra layer. verranno aggiunti inter layer nel pezzo del "collasso"
    edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei)
    {
        inner_edge_t e; bool b;
        if(g[boost::source(*ei,g)].layer!=g[boost::target(*ei,g)].layer)
        {
            boost::tie(e,b) = boost::add_edge(boost::source(*ei,g),boost::target(*ei,g),ig);
            ig[e].priority= g[*ei].priority;
            PRINT_DEBUG(boost::target(e,ig));

        /*if (v.second.get_child_optional("priority")){
            local_graph[e].priority=int_to_Priority(v.second.get_child("priority").get_value<int>());
        }
        else
        {
            local_graph[e].priority=int_to_Priority(NO_PRIORITY);
        }
        if (v.second.get_child_optional("to.port")){
         edge_to_port_map.insert(std::make_pair<edge_t,int>(e,v.second.get_child("to.port").get_value<int>()));
        }*/
            boost::tie(e,b) = boost::add_edge(boost::target(*ei,g),boost::source(*ei,g),ig);
            ig[e].priority= g[*ei].priority;
            PRINT_DEBUG(boost::target(e,ig));
        }
    }

    //filtra grafo esterno: tieni solo 4 livello e edge interni.
    //itera su vertici. se lay ==4 && edges.count == 1 then collassa.
}
