#include <iostream>
#include <fstream>
#include <sstream>
#include "params.hh"
#include "mystr.hh"

map<string, string> Params::params_;

bool Params::load (const string & path)
{
	params_["data_path"] = path;
	ostringstream filename;
	filename << path << "/params.txt";
	ifstream file (filename.str ().c_str ());
	if (!file) {
		cout << "Mypuck: impossible to open: " << filename.str () << endl;
		return false;
    }
    else {
    	cout << "Mypuck: loading params from file: " << filename.str () << endl;
    }
	string key;
	string value;
	while (!file.eof ()) {
      file >> key >> value;
      params_[key] = value;
    }
    file.close ();
//	params->show ();
	return true;
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

string Params::get_path ()
{
	static const int simu_id = Params::get_int ("SIMULATION_ID");
	ostringstream path;
	path << get ("data_path") << "/data_raw/" << simu_id << "/";
	return path.str ();
}

string Params::get_path_pc ()
{
	ostringstream path;
	path << get ("data_path") << "/tolman_files/";
	return path.str ();
}
