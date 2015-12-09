#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "iNARKConfig.h"

#include "graph/graph.h"
#include "graph/internal_graph.h"
int main (int argc, char *argv[])
{
#ifdef DEBUG
    std::cout<<"asd"<<std::endl;
#endif

    custom_graph g = custom_graph();
    g.create_graph("aaa");

    internal_graph ig = internal_graph(g.get_graph());
    if (g.search_component_dependences("task_2", "task_1"))
        return 0;
    else return 1;


}
