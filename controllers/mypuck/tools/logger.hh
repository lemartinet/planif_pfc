#ifndef LOGGER_HH_
#define LOGGER_HH_

#include "log.hh"
#include <map>
#include <string>

class ComputeUnit;

using namespace std;

class Logger
{
public:
	static bool open_logs ();
	static void close_logs ();
	static void add (const string& type, const ComputeUnit* unit);
	static void log (int step);
	static void log (const string& type, int step, const string& msg);
	static void log (const string& type, const string& msg);
	static void logw (const string& type);
	
private:
	static map<string, Log*> logs_;
	static map<string, Log*> free_logs_;
};

#endif /*LOGGER_HH_*/
