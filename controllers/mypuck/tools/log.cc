#include "log.hh"
#include "params.hh"
#include <fstream>
#include <sstream>
#include <string>

Log::Log (const string& dir, int no): dir_(dir), no1_(no), twono_(false), 
	file_(0), current_day_(0), current_trial_(0)
{
}

Log::Log (const string& dir, int no1, int no2): dir_(dir), no1_(no1), no2_(no2), twono_(true), 
	file_(0), current_day_(0), current_trial_(0)
{
}

Log::~Log()
{
	delete file_;
}

void Log::log (int day, int trial, const string& message)
{
	// on ouvre un seul fichier a la fois
	if (day != current_day_ || trial != current_trial_) {
		if (file_) {
			file_->close ();
			delete file_;
		}
		ostringstream path;
		path << Params::get_path () << dir_ << "/";
		if (twono_) {
			path << no1_ << "_" << no2_;
		}
		else {
			path << no1_;
		}
		path << "_d" << day << "_t" << trial << ".txt";
		file_ = new ofstream (path.str ().c_str ());
		current_day_ = day;
		current_trial_ = trial;
		if (file_->fail ()) {
			printf("error on %s\n", path.str ().c_str ());
		}
	}
	*file_ << message;
}
