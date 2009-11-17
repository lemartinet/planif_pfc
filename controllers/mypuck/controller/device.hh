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
	int* ps_value_get () const { return ps_value_; }
	double dist_goal_get () const { return dist_goal_; }
	bool sleep_get () const { return sleep_; }
	void speed_set(double left, double right) { setSpeed(left, right); }

	void synch ();
	float process_camera_image(const unsigned char *image, int *npixels);
	bool goal_reached () const;
	
private:
	DistanceSensor** ps_;
	int* ps_value_;
	Camera* camera_;
	GPS* gps_;
	Compass* compass_;
//	LightSensor* ls_;
	Receiver* receiver_;
	Coord position_;
	double orientation_;
	int num_pixels_goal_;
	bool goal_reached_;
	double dist_goal_;
	bool sleep_;
};

#endif
