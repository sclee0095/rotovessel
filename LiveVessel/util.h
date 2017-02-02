/*
util.h
2016 12 01
*/

#pragma once


#include <stdio.h>


//extern FILE *log_stream;

void InitLogStream(char *path, bool b_reinit=false);
void WriteLog(char *_file, int _line, char *_ftn = NULL, char *log = NULL);
//void WriteLog(char *_file, int _line, char *log=NULL);
void CloseLogStream();