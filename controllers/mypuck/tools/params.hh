#ifndef PARAMS_HH
# define PARAMS_HH

#include <string>
#include <map>

using namespace std;

class Params
{
public:
	static void load ();

  	static string get (string name);
//  	static void set (string name, int val);
	static double get_double (string name);
	static int get_int (string name);
	static string get_path ();
	static string get_path_pc ();
	  
  	static void show ();

private:
  	static map<string, string> params_;
  	static bool loaded_;
};

#endif
