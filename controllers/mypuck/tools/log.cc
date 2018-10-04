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

Log::Log (const string& type, bool wlog) throw (string)
try : wlog_(wlog), file_(0)
{
	string path = Params::get_path () + type + ".txt";
	file_ = new ofstream (path.c_str ());
	if (!*file_) {
		throw "Mypuck: impossible to open: " + path;
    }
	*file_ << unitbuf;
}
catch (string)
{
	delete file_;	
}

Log::~Log()
{
	delete file_;
}

void Log::log (int step) const
{
	if (!wlog_) {
		*file_ << step << " ";
		for_each (units_.begin (), units_.end (), var(*file_) << bind (&ComputeUnit::output, _1) << " ");
		*file_ << endl;
	}
}

void Log::logw () const
{
	static const double LOGW = Params::get_int("LOGW");
	if(wlog_ && LOGW == 1) {
		vector<const ComputeUnit*>::const_iterator it;
		for (it = units_.begin (); it != units_.end (); it++) {			
			vector<const ComputeUnit*>::const_iterator it2;
			for (it2 = units_.begin (); it2 != units_.end (); it2++) {
				const Neuron* n = dynamic_cast<const Neuron*>(*it2);
				Synapse* s;
				if (n && (s = n->syn_get (**it))) {
					*file_ << s->w_get () << " ";
				}
				else {
					*file_ << 0 << " ";
				}
			}
			*file_ << endl;
		}
	}	
}

void Log::log (int step, const string& msg) const
{
	*file_ << step << " " << msg << endl;
}

void Log::log (const string& msg) const
{
	*file_ << msg << endl;
}

void Log::add (const ComputeUnit* unit)
{
	units_.push_back (unit);
}

 
