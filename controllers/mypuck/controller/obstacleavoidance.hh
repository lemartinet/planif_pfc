#ifndef OBSTACLEAVOIDANCE_HH_
#define OBSTACLEAVOIDANCE_HH_

#include <vector>

// OAM state
enum oam_state { OAM_OFF = 0, OAM_ON = 1, OAM_STUCK = 2, OAM_BIGTURN = 3 };

class Coord;

using namespace std;

class ObstacleAvoidance
{
public:
	ObstacleAvoidance (int* value);
	virtual ~ObstacleAvoidance ();

	bool bloque_get () const { return oam_state_ == OAM_STUCK; }
	void avoid (double angle, int& left_speed, int& right_speed);
	void free_ways (vector<double>& dirs, double robot_angle);

private:
	int* ps_value;
	enum oam_state oam_state_;
};

#endif /*OBSTACLEAVOIDANCE_HH_*/
