#ifndef OBSTACLEAVOIDANCE_HH_
#define OBSTACLEAVOIDANCE_HH_

#include <webots/DifferentialWheels.hpp>
#include <vector>

using namespace webots;

class Coord;
class RobotDevice;

using namespace std;

class ObstacleAvoidance
{
public:
	ObstacleAvoidance (RobotDevice& robot);
	virtual ~ObstacleAvoidance ();
	
	float obstacle_left_get () const { return obstacle_left_; }
	float obstacle_right_get () const { return obstacle_right_; }
	float obstacle_left_back_get () const { return obstacle_left_back; }
	float obstacle_right_back_get () const { return obstacle_right_back; }
	float obstacle_left_mid_get () const { return obstacle_left_mid; }
	float obstacle_right_mid_get () const { return obstacle_right_mid; }
	float obstacle_left_front_get () const { return obstacle_left_front; }
	float obstacle_right_front_get () const { return obstacle_right_front; }
	int dist_sensor_get () const;
	
	void update_info ();
	void avoid_block (const Coord& position);
	bool avoid (double angle, const Coord& position, int& left_speed, int& right_speed);
	int analyse_cross_road (bool& left, bool& straight, bool& right);
	void free_ways (vector<double>& dirs, double robot_angle);
	
private:
	DistanceSensor** ps_;
	float obstacle_left_front;
	float obstacle_right_front;
	float obstacle_left_mid;
	float obstacle_right_mid;
	float obstacle_left_;
	float obstacle_right_;
	float obstacle_left_back;
	float obstacle_right_back;
	bool left_near_;
	bool right_near_;
};

#endif /*OBSTACLEAVOIDANCE_HH_*/
