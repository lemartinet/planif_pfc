#ifndef LOG_HH_
#define LOG_HH_

#include <vector>
#include <string>
#include <fstream>

class ComputeUnit;

using namespace std;

class Log
{
public:
	Log (const string& type, bool wlog=false) throw (string);
	virtual ~Log();
	
	void log (int step) const;
	void logw () const;
	void log (const string& msg, int step) const;
	void log (const string& msg) const;
	void add (const ComputeUnit* unit);
	
private:
	bool wlog_;
	ofstream* file_;
	vector<const ComputeUnit*> units_;
};

#endif /*LOG_HH_*/
