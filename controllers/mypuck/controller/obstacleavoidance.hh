#ifndef OBSTACLEAVOIDANCE_HH_
#define OBSTACLEAVOIDANCE_HH_

class DifferentialWheels;
class DistanceSensor;
class Coord;

class ObstacleAvoidance
{
public:
	ObstacleAvoidance();
	virtual ~ObstacleAvoidance();
	
	float obstacle_left_get () const { return obstacle_left_; }
	float obstacle_right_get () const { return obstacle_right_; }
	float obstacle_left_back_get () const { return obstacle_left_back; }
	float obstacle_right_back_get () const { return obstacle_right_back; }
	float obstacle_left_mid_get () const { return obstacle_left_mid; }
	float obstacle_right_mid_get () const { return obstacle_right_mid; }
	float obstacle_left_front_get () const { return obstacle_left_front; }
	float obstacle_right_front_get () const { return obstacle_right_front; }
	int dist_sensor_get () const;
	
	void update_obstacle_info ();
	void avoid_block (const Coord& position);
	bool obstacle_avoidance_module (double angle, const Coord& position);
	
private:
	DifferentialWheels& robot_;
	DistanceSensor** ps_;
	float obstacle_left_front;
	float obstacle_right_front;
	float obstacle_left_mid;
	float obstacle_right_mid;
	float obstacle_left_;
	float obstacle_right_;
	float obstacle_left_back;
	float obstacle_right_back;
};

#endif /*OBSTACLEAVOIDANCE_HH_*/
