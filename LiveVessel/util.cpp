

#include "stdafx.h"
#include "util.h"
#include <string>

#include <fstream>


// log file stream
//FILE * log_stream = nullptr;
std::ofstream log_file;
// number of log lines
int log_num_lines = 0;
// log file stream path
//std::string log_stream_path;
char log_stream_path[200];

#define MAX_LOG_NUM_LINES		100000

// initialize log file stream
void InitLogStream(char *path, bool b_reinit)
{
	// store log path
	//log_stream_path = std::string(path);
	sprintf(log_stream_path, "%s", path);
	// if log stream is open, close now
	//if (log_stream) {
	//	CloseLogStream();
	//}

	log_num_lines = 0;
	if (log_file.is_open())
		log_file.close();
	log_file.open(log_stream_path, std::ios_base::out | std::ios_base::trunc);

	if (b_reinit)
		log_file << "LOG RE-INITIALIZED" << std::endl;
	else 
		log_file << "LOG INITIALIZED" << std::endl;

	//// open file, write mode
	//fopen_s(&log_stream, path, "w");

	//// write initialization
	//fprintf_s(log_stream, "LOG INITIALIZED\n");
}

void WriteLog(char *_file, int _line, char *_ftn, char *log)
{
	//printf("%d\n", log_num_lines);
	// to prevent log file size from becoming to large
	log_num_lines++;
	if (log_num_lines > MAX_LOG_NUM_LINES)
		InitLogStream(log_stream_path);
	if (_file)	
		log_file << "FILE: " << std::string(_file) << "\t";
	log_file << "LINE: " << _line;
	if (_ftn)
		log_file << "\t" << "function: " << std::string(_ftn) << "\t";
	if (log)
		log_file << "\t" << ":" << std::string(log);
	log_file << std::endl;
}

void WriteLog(char *log)
{
	log_num_lines++;
	if (log_num_lines > MAX_LOG_NUM_LINES)
		InitLogStream(log_stream_path);
	if (log)
		log_file << std::string(log);
	log_file << std::endl;
}

void CloseLogStream()
{
	//// open file, write mode
	//if (log_stream) {
	//	// write end
	//	fprintf_s(log_stream, "LOG END\n");

	//	fclose(log_stream);
	//	log_stream = nullptr;
	//}
}
