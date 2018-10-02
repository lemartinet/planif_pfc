#include "log.hh"
#include "params.hh"
#include "computeunit.hh"
#include "neuron.hh"
#include <iostream>
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
		int nb_all = ComputeUnit::nb_computeunits_get ();
		double** w_all = new double* [nb_all];
		for (int i = 0; i < nb_all; i++) {
			w_all[i] = new double [nb_all];
			for (int j = 0; j < nb_all; j++) {
				w_all[i][j] = 0;
			}
		}
		vector<const ComputeUnit*>::const_iterator it;
		for (it = units_.begin (); it != units_.end (); it++) {
			const Neuron* n = dynamic_cast<const Neuron*>(*it);
			if (n == 0) {
				continue;
			}
			const map<const int, Synapse *>& mw = n->all_syn_get ();			
			map<const int, Synapse *>::const_iterator it2;
			for (it2 = mw.begin (); it2 != mw.end (); it2++) {
				int no = it2->first;
				Synapse* s = it2->second;
				w_all[no][n->no_get ()] = s->w_get () * s->a_get ();
				if (s->is_mult ()) {
					w_all[s->from_mult_get ()->no_get ()][n->no_get ()] = s->w_get () * s->b_get ();
				}
			}
		}
		for (int i = 0; i < nb_all; i++) {
			for (int j = 0; j < nb_all; j++) {
				*file_ << w_all[i][j] << " ";
			}
			*file_ << endl;
			delete [] w_all[i];
		}
		*file_ << endl << endl;
		delete [] w_all;
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

 
