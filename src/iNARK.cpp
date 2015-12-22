#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "iNARKConfig.h"

#include "graph/graph.h"
#include "graph/internal_graph.h"
int main (int argc, char *argv[])
{
  //TODO destructors
  try{
      #ifdef DEBUG
    std::cout<<"asd"<<std::endl;
#endif

    custom_graph g = custom_graph();
    g.create_graph("aaa");

    internal_graph ig = internal_graph(g.get_graph());
    if (ig.search_path("OS_2", "task_1",SAFETY_CRITICAL,CONTROLLER))
        return 0;
    else return 100;

  }
   catch(const std::exception&)  // Consider using a custom exception type for intentional throws. A good idea might be a `return_exception`.
    {                             
        return EXIT_FAILURE;
    }


}
