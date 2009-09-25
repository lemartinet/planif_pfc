/*
 * File:         testy.hh
 * Date:         December 2007
 * Description:  class for all the outputs
 * Author:       Benjamin Fouque, louis-emmanuel.martinet@lip6.fr
 */

#ifndef   	TESTY_HH_
#define   	TESTY_HH_

#include <fstream>
#include <vector>

using namespace std;

#define     TESTY_TIMESTEP_WEIGHTS        500
#define     TESTY_TIMESTEP_OUTPUTS        10
#define     TESTY_TIMESTEP_OUTPUTS_CYCLE  5000
#define     TESTY_TIMESTEP_CELLS          10

class RobotDevice;
class Neurosolver;
class Behavior;

class Testy
{
public:
	Testy (RobotDevice& robot, Behavior& behav, Neurosolver& solver);
	~Testy ();

	void init ();
	void synch ();
	void end_simu ();
	double weights_sum_mean ();
	double weights_sum_max ();

private:
	vector<ofstream*> files_weights_;
	vector<ofstream*> files_outputs_;
	vector<ofstream*> files_cells_;
  	RobotDevice*  robot_;
  	Behavior* behav_;
  	Neurosolver* solver_;
	double weights0_;
  	double weights1_;
  	int cpt_;
  	int cpt_goal;
  	int simu_id_;
};

#endif	    /* !TESTY_HH_ */
