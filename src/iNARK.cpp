#include "graph/graph.h"
int main (int argc, char *argv[])
{
    custom_graph g = custom_graph();
    g.create_graph("aaa");
    if (g.filter_and_explore_graph(MISSION_CRITICAL,"task_0", "task_1"))
        return 0;
    else return 1;


}
