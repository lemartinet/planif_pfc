#include "log.hh"
#include "params.hh"
#include "computeunit.hh"
#include "neuron.hh"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <algorithm>

using namespace boost::lambda;

Log::Log (const string& type, bool wlog) : type_(type), wlog_(wlog), file_(0)
{
	if (!wlog_) {
		ostringstream path;
		path << Params::get_path () << type_ << ".txt";
		file_ = new ofstream (path.str ().c_str ());
		*file_ << unitbuf;
	}
}

Log::~Log()
{
	if (!wlog_) {
		delete file_;
	}
	else {
		for (unsigned int i = 0; i < wfile_.size(); i++) {
			delete wfile_[i];	
		}	
	}
}

void Log::log (int step) const
{
	if (!wlog_) {
		*file_ << step << " ";
		for_each (units_.begin (), units_.end (), var(*file_) << bind (&ComputeUnit::output, _1) << " ");
		*file_ << endl;
	}
	else {
		for (unsigned int i = 0; i < wfile_.size(); i++) {
			*wfile_[i] << step << " ";
			dynamic_cast<const Neuron*>(units_[i])->print_weights (*wfile_[i]);
			*wfile_[i] << endl;
		}
	}
}

void Log::log (int step, const string& msg) const
{
	*file_ << step << " " << msg << endl;
}

void Log::add (const ComputeUnit* unit)
{
	static const double LOGW = Params::get_int("LOGW");
	if(wlog_ && LOGW == 1) {
		ostringstream path;
		path << Params::get_path () << type_ << "/" << units_.size () << ".txt";
		ofstream* f = new ofstream (path.str ().c_str ());
		wfile_.push_back(f);
		*f << unitbuf;			
	}
	units_.push_back (unit);
}

 
