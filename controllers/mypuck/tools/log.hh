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
	Log (const string& type, bool wlog = false);
	virtual ~Log();
	
	void log (int step) const;
	void log (int step, const string& msg) const;
	void add (const ComputeUnit* unit);
	
private:
	const string type_;
	bool wlog_;
	ofstream* file_;
	vector<ofstream*> wfile_;
	vector<const ComputeUnit*> units_;
};

#endif /*LOG_HH_*/
