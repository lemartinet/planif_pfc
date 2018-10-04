#include "logger.hh"
#include "computeunit.hh"
#include "params.hh"
#include <iostream>

map<string, Log*> Logger::logs_;
map<string, Log*> Logger::free_logs_;

void Logger::open_logs ()
{
	logs_["pc"] = new Log("pc");
	logs_["inf"] = new Log("inf");
	logs_["sup"] = new Log("sup");
	logs_["state"] = new Log("state");
	logs_["lat"] = new Log("lat");
	logs_["maxsup"] = new Log("maxsup");
	logs_["weights"] = new Log("weights", true);
	free_logs_["decision"] = new Log("decision");
	free_logs_["network"] = new Log("network");
}

void Logger::close_logs ()
{
	for (map<string, Log*>::iterator it = logs_.begin (); it != logs_.end (); ++it) {
		delete it->second;
	}
}

//void Logger::new_log (const string& type)
//{
//	logs_[type] = new Log(type);
//}

void Logger::add (const string& type, const ComputeUnit* unit)
{
	if (logs_.find (type) != logs_.end()) {
		logs_[type]->add(unit);
	} 
	else {
		cout << "Mypuck: error: log type " << type << " not found" << endl;
	}	
}

void Logger::log (int step)
{
	// si on est en mode echantillonage des activites, 
	// on enregistre ts les TIMESTEP_OUTPUTS pas de temps
	static const int LOG = Params::get_int("LOG");
	static const int TIMESTEP_OUTPUTS = Params::get_int("TIMESTEP_OUTPUTS");
	if (LOG != 1 || step % TIMESTEP_OUTPUTS != 0) {
		return;	
	}

	for (map<string, Log*>::iterator it = logs_.begin (); it != logs_.end (); ++it) {
		it->second->log (step);
	}	
}

void Logger::log (const string& type, int step, const string& msg)
{
	if (free_logs_.find (type) != free_logs_.end()) {
		free_logs_[type]->log(step, msg);
	} 
	else {
		cout << "Mypuck: error: log type " << type << " not found" << endl;
	}
}

