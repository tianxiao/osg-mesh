#include "stdafx.h"
#include "xtLog.h"

std::ofstream g_log_file;

bool Start_Log( const char *file_name) 
{
	g_log_file.open(file_name);
	if ( !g_log_file.is_open() ) {
		printf("ERROR: could not open log file % for writing\n",file_name);
		return false;
	}
	g_log_file << "XT log file "<< std::endl;
	return true;
}

void Close_Log()
{
	g_log_file.flush();
	g_log_file.close();
}
