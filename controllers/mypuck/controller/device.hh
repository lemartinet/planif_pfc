#ifndef DEVICE_HH
# define DEVICE_HH

#include "coord.hh"
#include <webots/DifferentialWheels.hpp>
#include <fstream>

using namespace webots;

class RobotDevice : public DifferentialWheels
{
public:
	RobotDevice ();
	~RobotDevice ();
	
	double angle_get () const { return orientation_; }
	const Coord& position_get () const { return position_; }
	bool manually_moved () const { return manually_moved_; }
	int nb_trial_get () const { return nb_trial_; }
	int cpt_trial_get () const { return cpt_trial_; }
	int cpt_total_get () const { return cpt_total_; }
	int day_get () const { return 1 + nb_trial_ / 12; } // il y a 12 essais par jour (sauf le dernier)
	int trial_get () const { return 1 + nb_trial_ % 12; } 

	void synch ();
	bool goal_reached () const;
//	double distance_goal_factor () const;
//	bool all_ways_visited (bool reinit = false);
	void write_message (ofstream& file, const string& message) const;
	void output_mypuck (const string& message) const { write_message (*output_mypuck_, message); }
	void output_neuro (const string& message) const { write_message (*output_neuro_, message); }
	string time_get (bool total) const;
	
	int run ();

private:
	Camera* camera_;
	GPS* gps_;
	Compass* compass_;
//	LightSensor* ls_;
	Receiver* receiver_;
	bool manually_moved_;
	Coord position_;
	float orientation_;
	double goal_dir_;
	int num_pixels_goal_;
	int nb_trial_;
	bool goal_reached_;
	int cpt_trial_;
	int cpt_total_;
	ofstream* output_mypuck_;
	ofstream* output_neuro_;
};

#endif
