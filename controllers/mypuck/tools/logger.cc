#include "logger.hh"
#include "computeunit.hh"
#include "params.hh"
#include <iostream>

map<string, Log*> Logger::logs_;
map<string, Log*> Logger::free_logs_;

bool Logger::open_logs ()
{
	try {
		logs_["fr"] = new Log("fr");
		free_logs_["decision"] = new Log("decision");
		free_logs_["network"] = new Log("network");
		free_logs_["weight"] = new Log("weight", true);
		free_logs_["type"] = new Log("type");
		free_logs_["type"]->log("% neurone number | type");
		free_logs_["type"]->log("% type: 0=state, 1=inf, 2=lat, 3=sup, 4=maxsup, 10=pc, 11=dircell, 12=motiv");		
		free_logs_["col"] = new Log("col");
		free_logs_["col"]->log("% neurone number | column number");
		free_logs_["minicol"] = new Log("minicol");
		free_logs_["minicol"]->log("% neurone number | minicolumn number");
		free_logs_["minicol_col"] = new Log("minicol_col");
		free_logs_["minicol_col"]->log("% minicol number | column number");		
	}
	catch (string s) {
		cout << s << endl;
		return false;	
	}
	return true;
}

void Logger::close_logs ()
{
	for (map<string, Log*>::iterator it = logs_.begin (); it != logs_.end (); ++it) {
		delete it->second;
	}
	for (map<string, Log*>::iterator it = free_logs_.begin (); it != free_logs_.end (); ++it) {
		delete it->second;
	}
}

void Logger::add (const string& type, const ComputeUnit* unit)
{
	if (logs_.find (type) != logs_.end()) {
		logs_[type]->add(unit);
	}
	else if (free_logs_.find (type) != free_logs_.end()) {
		free_logs_[type]->add(unit);
	}
	else {
		cout << "Mypuck: error: log type " << type << " not found" << endl;
	}	
}

void Logger::log (int step)
{
	// si on est en mode echantillonage des activites, 
	// on enregistre ts les STEP_LOG pas de temps
	static const int LOG = Params::get_int("LOG");
	static const int STEP_LOG = Params::get_int("STEP_LOG");
	if (LOG != 1 || step % STEP_LOG != 0) {
		return;	
	}

	for (map<string, Log*>::iterator it = logs_.begin (); it != logs_.end (); ++it) {
		it->second->log (step);
	}	
}

void Logger::log (const string& type, int step, const string& msg)
{
	if (free_logs_.find (type) != free_logs_.end()) {
		free_logs_[type]->log (step, msg);
	} 
	else {
		cout << "Mypuck: error: log type " << type << " not found" << endl;
	}
}

void Logger::log (const string& type, const string& msg)
{
	if (free_logs_.find (type) != free_logs_.end()) {
		free_logs_[type]->log (msg);
	} 
	else {
		cout << "Mypuck: error: log type " << type << " not found" << endl;
	}
}

void Logger::logw (const string& type)
{
	if (free_logs_.find (type) != free_logs_.end()) {
		free_logs_[type]->logw ();
	} 
	else {
		cout << "Mypuck: error: log type " << type << " not found" << endl;
	}
}

