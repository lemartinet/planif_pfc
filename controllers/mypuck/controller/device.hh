#ifndef DEVICE_HH
# define DEVICE_HH

#include "coord.hh"
#include <webots/DifferentialWheels.hpp>
#include <webots/Robot.hpp>

class Camera;
class GPS;

class RobotDevice : public DifferentialWheels
{
public:
	RobotDevice ();
	~RobotDevice () {} 

	void speed_set (float v1, float v2);
	bool is_moving_get() const {return is_moving_;}
	float angle_get () const { return orientation_; }
	const Coord& position_get () const { return position_; }
	bool goal_reached_prec () const { return goal_reached_prec_;}
	bool manually_moved () const { return manually_moved_; }

	void synch ();
	bool goal_reached () const;
	double distance_goal_factor () const;
	bool all_ways_visited (bool reinit = false);

private:
	Camera* camera_;
	GPS* gps_;
	bool synch_;
	bool is_moving_;
	bool manually_moved_;
	bool goal_reached_prec_;
	Coord position_;
	float orientation_;
	double goal_dir_;
	int num_pixels_goal_;
};

#endif
