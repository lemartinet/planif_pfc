#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "params.hh"
#include "mystr.hh"

#define PARAMS_PATH "../../data/"

map<string, string> Params::params_;
bool Params::loaded_ = false;

void Params::load ()
{
	string path = PARAMS_PATH;
	params_["data_path"] = path;
	ostringstream filename;
	filename << path << "/params.txt";
	ifstream file (filename.str ().c_str ());
	if (!file) {
		cout << "Mypuck: impossible to open: " << filename.str () << endl;
		cout << "Mypuck: erreur d'init. Exit !" << endl;
		exit (-1);	
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
	Params::loaded_ = true;
//	Params::show ();
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
	if (! Params::loaded_) {
		Params::loaded_ = true;
		Params::load ();
//		cout << "loaded" << endl;
	}
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


//void Params::set (string name, int val)
//{
//	ostringstream str;
//	str << val;
//	params_[name] = str.str();
//}

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
