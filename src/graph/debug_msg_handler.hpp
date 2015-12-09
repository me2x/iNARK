#include "iNARKConfig.h"
#include <iostream>
#include <boost/lexical_cast.hpp>

#ifndef DEBUG_HANDLER
#define DEBUG_HANDLER

# ifdef DEBUG
# define PRINT_DEBUG(mex) \
    std::cout << mex <<std::endl
# else
# define PRINT_DEBUG(mex) void (0)
# endif


#endif
