#ifndef OBSTACLEAVOIDANCE_HH_
#define OBSTACLEAVOIDANCE_HH_

#include <vector>

class Coord;
class RobotDevice;

using namespace std;

class ObstacleAvoidance
{
public:
	ObstacleAvoidance (RobotDevice& robot);
	virtual ~ObstacleAvoidance ();

	bool bloque_get () const { return bloque_; }
	void avoid (double angle, const Coord& position, int& left_speed, int& right_speed);
	void free_ways (vector<double>& dirs, double robot_angle);

private:
	void avoid_block (const Coord& position);
	int analyse_cross_road (bool& left, bool& straight, bool& right);

private:
	int* ps_value;
	bool left_near_;
	bool right_near_;
	bool bloque_;
	bool demi_tour;
	int blocked;
	double last_x, last_y;
};

#endif /*OBSTACLEAVOIDANCE_HH_*/
