#include <iostream>
#include <fstream>
#include <sstream>
#include "params.hh"
#include "mystr.hh"

Params::Params (const string & filename)
{
	ifstream file (filename.c_str ());
	string key;
	string value;
	cout << "Mypuck: loading params from file: " << filename << endl;
	
	if (!file) {
		cout << "Mypuck: impossible to open: " << filename << endl;
		return;
    }
	while (!file.eof ()) {
      file >> key >> value;
      params_[key] = value;
    }
    file.close ();
}

void Params::show ()
{
  for (map<string, string>::iterator it = params_.begin (); it != params_.end (); ++it)
    {
    	cout << it->first << " " << it->second << endl;
    }
}

string Params::get (string name) 
{ 
	if (params_.count(name) > 0) {
		return params_[name];
	} 
	else {
		stringstream s;
		s << "Mypuck: error: param " << name << " not found" << endl;
		cout << s.str() << endl;
		return string();
	} 
}
  
double Params::get_double (string name) 
{ 
	return str2d (get (name)); 
}
  
int Params::get_int (string name) 
{ 
	return str2i (get (name)); 
}
