//#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
//#include "preprocessor.h"

//The fem project needed
#include <string>
#include <iostream>

//The setw() header file
# include <iomanip>

# include <cstdlib>
# include <istream>
# include <iomanip>
# include <fstream>
# include <ctime>
# include <cmath>
# include <cstring>


//To do the match things
#include <regex>

//Add this lib to fullfill the split space
#include <sstream>

//Add the vector for  dirichlet condition
#include <vector>



// TODO: reference additional headers your program requires here

#define MYDEBUG 1


//Define the triangle MACRO
#define ANSI_DECLARATORS 


#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */


#define TMYDEBUG 0