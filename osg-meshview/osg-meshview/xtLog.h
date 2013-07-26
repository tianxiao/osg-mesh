#pragma once 
#ifndef _SOME_UNIQUE_NAME_H_
#define _SOME_UNIQUE_NAME_H_
//#include <ofstream>
//extern std::ofstream g_log_file;
#include <fstream>

extern std::ofstream g_log_file;

bool Start_Log( const char *file_name) ;
void Close_Log();

#endif