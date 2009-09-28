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
	double distance_goal_factor () const { return (dist_goal_ > 1)?1:dist_goal_; }
	bool sleep_get () const { return sleep_; }

	void synch ();
	float process_camera_image(const unsigned char *image, int *npixels);
	bool goal_reached () const;
	
	static const RobotDevice& robot_get () { return *the_robot_; }

private:
	static RobotDevice* the_robot_;
	Camera* camera_;
	GPS* gps_;
	Compass* compass_;
//	LightSensor* ls_;
	Receiver* receiver_;
	bool manually_moved_;
	Coord position_;
	double orientation_;
	int num_pixels_goal_;
	int nb_trial_;
	bool goal_reached_;
	int cpt_trial_;
	int cpt_total_;
	double dist_goal_;
	bool sleep_;
};

#endif
