#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "iNARKConfig.h"
#include "boost/program_options.hpp"

#include "graph/graph.hpp"
#include "graph/timing_internal_graph.hpp"
#include "graph/fault_tree_graph.h"

#include <memory>
int main (int argc, char *argv[])
{
  try
  {
    std::string input_graph_name = "not_defined";
    std::string from_component = "not_defined";
    std::string to_component = "not_defined";
    int search_type = 0 ;
    int search_depth = 0;
     
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
    ("help,h","print usage message")
    ("input,i",boost::program_options::value(&input_graph_name),"name of the file containing the xml graph")
    ("source",boost::program_options::value (&from_component),"name of the component whose interference has to be tested")
    ("target",boost::program_options::value (&to_component),"name of the component whose independence has to be tested")
    ("type",boost::program_options::value(&search_type),"type of the search to be performed: 1 for inteference of source toward target, 2 for possible interferences of source and 3 for possible interferences to target")
    ("depth",boost::program_options::value(&search_depth),"depth of the search: 1 for local OS search, 2 for component layer depth and 3 for physical layer search")
    ;
    
     boost::program_options::variables_map vm;
     boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);


   // timing_internal_graph ig = timing_internal_graph(g.get_graph());
   // if (ig.search_path("task_1", "task_2",SAFETY_CRITICAL,RESOURCE))

        if (vm.count("help")) {  
            std::cout << "Usage: provide one of all the following options: "<<std::endl;
            std::cout << "input, source, target, type, depth"<< std::endl<< std::endl;
            std::cout << desc << "\n";
            return 0;
        }
        if (vm.count("input")) {  
            input_graph_name = vm["input"].as<std::string>();
        }
        if (vm.count("source")) {  
            from_component = vm["source"].as<std::string>();
        }
        if (vm.count("target")) {  
            to_component = vm["target"].as<std::string>();
        }
        if (vm.count("type")) {  
            search_type = vm["type"].as<int>();
        }
        if (vm.count("depth")) {  
            search_depth = vm["depth"].as<int>();
        }

        source_graph g = source_graph();
        if (input_graph_name != "not_defined")
        {
    #ifdef TIME 
        std::cout << "graph creation time is (ns): "<<measure<std::chrono::nanoseconds>::execution( [&]() { g.create_graph_from_xml(input_graph_name);})<<std::endl;
    #else
            g.create_graph_from_xml(input_graph_name);
    #endif
        }
        else 
        {
            std::cout<<std::endl<<std::endl<<"\t\t\tinput file not found"<<std::endl<<std::endl;
            std::cout << desc << "\n";
            
            return EXIT_FAILURE;
        }
        
        Layer l = LAYER_ERROR;
        switch (search_depth)
        {
        case 0: 
        {
            std::cerr<<"research depth not defined"<<std::endl;
    #ifdef DEBUG	
            std::cout<<"research depth not defined"<<std::endl;
    #endif	
            return EXIT_FAILURE;
            break;
        }
        case 1:
        {
            l = CONTROLLER;
            break;
        }
        case 2:
        {
            l = RESOURCE;
            break;
        }
        case 3:
        {
            l = PHYSICAL;
            break;
        }
        default:
        {
            std::cerr<<"error: search depth value not valid"<<std::endl;
    //#ifdef DEBUG	
            std::cout<<"error: search depth value not valid"<<std::endl;
    //#endif	
            return EXIT_FAILURE;
            break;
        }
        }
        timing_internal_graph ig = timing_internal_graph();
        std::shared_ptr<Source_Graph> tmp = g.get_source_graph_ref();
        
    #ifdef TIME 
        std::cout << "graph creation time is (ns): "<<measure<std::chrono::nanoseconds>::execution( [&]() { ig.build_graph(g.get_source_graph_ref());})<<std::endl;
    #else
        ig.build_graph(g.get_source_graph_ref());
    #endif

        
        switch (search_type)
        {
            case 0: 
        {
            std::cerr<<"research type not defined"<<std::endl;
    #ifdef DEBUG	
            std::cout<<"research type not defined"<<std::endl;
    #endif	
            return EXIT_FAILURE;
            break;
        }
        case 1:
        {
            
            bool search_result_configuration_OK;
            
    #ifdef TIME          
            std::cout << "graph exploration time is (ns): "<<measure<std::chrono::nanoseconds>::execution( [&]() {search_result_configuration_OK = ig.search_path(from_component, to_component,l);})<<std::endl;
    #else	
            search_result_configuration_OK = ig.search_path(from_component, to_component,l);
    #endif
            if (search_result_configuration_OK)
            {
                std::cout <<"no path found"<<std::endl;    
                return 0;
            }
            else
            {
                std::cout<<"path found" <<std::endl;
                return 100;
            }
            break;
        }
        case 2:
        {
            fault_tree_graph ftg = fault_tree_graph();
            ftg.build_graph(g.get_source_graph_ref());
    #ifdef DEBUG	
            std::cout<<"not implemented yet"<<std::endl;
    #endif	
            return 0;
            break;
        }
        case 3:
        {
            //ig.search_interfered_nodes(to_component, true);
    //#ifdef DEBUG	
            std::cout<<"not implemented yet"<<std::endl;
    //#endif
            return ENOSYS;            
            break;
        }
        default:
        {
            std::cerr<<"error: search type value not valid"<<std::endl;
    //#ifdef DEBUG	
            std::cout<<"error: search type value not valid"<<std::endl;
    //#endif	
            return EXIT_FAILURE;
            break;
        }
        
        }

  }
  catch(const std::exception& e)  // Consider using a custom exception type for intentional throws. A good idea might be a `return_exception`.
  {   
    std::cerr<< e.what() << std::endl;
#ifdef DEBUG      
    std::cout<< e.what() << std::endl;
#endif      
    return EXIT_FAILURE;
  }

    
}
