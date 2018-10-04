#ifndef LOG_HH_
#define LOG_HH_

#include <string>
#include <fstream>

using namespace std;

class Log
{
public:
	Log ();
	Log (const string& dir, int no);
	Log (const string& dir, int no1, int no2);
	
	virtual ~Log();
	
	void log (int day, int trial, const string& message);
	
private:
	string dir_;
	int no1_;
	int no2_;
	bool twono_;
	ofstream* file_;
	int current_day_;
	int current_trial_;
};

#endif /*LOG_HH_*/
