#ifndef PARAMS_HH
# define PARAMS_HH

#include <string>
#include <map>

using namespace std;

class Params
{
public:
	Params (const string & file);

  	string get (string name);
	double get_double (string name);
	int get_int (string name);
  
  	void show ();

private:
  	map<string, string> params_;
};

#endif
