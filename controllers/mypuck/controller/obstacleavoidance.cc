#include "obstacleavoidance.hh"
#include "device.hh"
#include "params.hh"
#include "coord.hh"
#include "math.hh"
#include <cmath>
#include <webots/DifferentialWheels.hpp>
#include <webots/DistanceSensor.hpp>

// 8 IR proximity sensors
#define NB_SENSORS 8
#define NB_DIST_SENS 8
#define PS_RIGHT_00 0
#define PS_RIGHT_45 1
#define PS_RIGHT_90 2
#define PS_RIGHT_REAR 3
#define PS_LEFT_REAR 4
#define PS_LEFT_90 5
#define PS_LEFT_45 6
#define PS_LEFT_00 7
#define NO_SIDE -1
#define LEFT 0
#define RIGHT 1

int ps_value[NB_SENSORS] = {0,0,0,0,0,0,0,0};
const int PS_OFFSET_SIMULATION[NB_DIST_SENS] = {300,300,300,300,300,300,300,300};

////////////////////////////////////////////
// OAM - Obstacle Avoidance Module (from e-puck_line controller)
//
// The OAM routine first detects obstacles in front of the robot, then records
// their side in "oam_side" and avoid the detected obstacle by 
// turning away according to very simple weighted connections between
// proximity sensors and motors. "oam_active" becomes active when as soon as 
// an object is detected and "oam_reset" inactivates the module and set 
// "oam_side" to NO_SIDE. Output speeds are in oam_speed[LEFT] and oam_speed[RIGHT].

bool oam_active;
int oam_speed[2];
int oam_side = NO_SIDE;
bool bloque;
bool demi_tour;

#define OAM_OBST_THRESHOLD 3000
#define OAM_FORWARD_SPEED 400
#define OAM_K_PS_90 0.02
#define OAM_K_PS_45 0.09
#define OAM_K_PS_00 0.12
#define OAM_K_MAX_DELTAS 600
#define OAM_BLOQUE 8000

int blocked = 0;
double last_x = 0, last_y = 0;


ObstacleAvoidance::ObstacleAvoidance (RobotDevice& robot):
	left_near_(false), right_near_(false)
{
	static const int TIME_STEP = Params::get_int("TIME_STEP");
	ps_ = new DistanceSensor*[NB_SENSORS];
	char name[4]="ps0";
	for (int i = 0; i < NB_SENSORS; i++) {
		ps_[i] = robot.getDistanceSensor (name);
		ps_[i]->enable (TIME_STEP);
		name[2]++;
    }
}

ObstacleAvoidance::~ObstacleAvoidance ()
{
	delete[] ps_;
}

void ObstacleAvoidance::avoid_block (const Coord& position) 
{
  	double x = position.x_get ();
	double y = position.y_get ();
  
  	if (fabs (last_x - x) < 0.002 && fabs (last_y - y) < 0.002) {
    	blocked++;
  	}
  	else {
    	blocked = blocked > 0 ? blocked - 1: 0;
  	}
  	last_x = x;
  	last_y = y;
}

void ObstacleAvoidance::update_info ()
{
	obstacle_left_front = ps_[7]->getValue ();
  	obstacle_right_front = ps_[0]->getValue ();
  	obstacle_left_mid = ps_[6]->getValue ();
  	obstacle_right_mid = ps_[1]->getValue ();
  	obstacle_left_ = ps_[5]->getValue ();
  	obstacle_right_ = ps_[2]->getValue ();
  	obstacle_left_back = ps_[4]->getValue ();
  	obstacle_right_back = ps_[3]->getValue ();
	for (int i=0; i < NB_DIST_SENS; i++) {
		int val = (int)ps_[i]->getValue ();
	  	ps_value[i] = ((val - PS_OFFSET_SIMULATION[i]) < 0)?0:(val - PS_OFFSET_SIMULATION[i]);
	}
}

bool ObstacleAvoidance::avoid (double angle, const Coord& position, int& left_speed, int& right_speed)
{
	update_info ();
	
  int max_ds_value, DeltaS=0, i;
  int Activation[]={0,0};

  // Determine the presence and the side of an obstacle
  max_ds_value = 0;
  for (i = PS_RIGHT_00; i <= PS_RIGHT_45; i++) {
    if (max_ds_value < ps_value[i]) max_ds_value = ps_value[i];
  	Activation[RIGHT] += ps_value[i];
  }
  Activation[RIGHT] += ps_value[PS_RIGHT_90];
  for (i = PS_LEFT_45; i <= PS_LEFT_00; i++) {
    if (max_ds_value < ps_value[i]) max_ds_value = ps_value[i];
    Activation[LEFT] += ps_value[i];
  }
  Activation[LEFT] += ps_value[PS_LEFT_90];
  if (max_ds_value > OAM_OBST_THRESHOLD) {
  	oam_active = true;
  }
  else {
  	oam_active = false;
  	bloque = false;
  }

  if (oam_active && !bloque) {
  	if (Activation[RIGHT] > OAM_BLOQUE && Activation[LEFT] > OAM_BLOQUE) {
  		bloque = true;
  		//cout << "bloque" << endl;
  	}
    if (Activation[RIGHT] > Activation[LEFT]) oam_side = RIGHT;
    else oam_side = LEFT;
  }

  // Forward speed
  oam_speed[LEFT]  = OAM_FORWARD_SPEED;
  oam_speed[RIGHT] = OAM_FORWARD_SPEED;

	// Go away from obstacle
  	if (!demi_tour && oam_active) {
    	// The rotation of the robot is determined by the location and the side of the obstacle
    	if (oam_side == LEFT) {
    		DeltaS -= (int) (OAM_K_PS_90 * ps_value[PS_LEFT_90]);
      		DeltaS -= (int) (OAM_K_PS_45 * ps_value[PS_LEFT_45]);
      		DeltaS -= (int) (OAM_K_PS_00 * ps_value[PS_LEFT_00]);
    	}
    	else { // oam_side == RIGHT
    		DeltaS += (int) (OAM_K_PS_90 * ps_value[PS_RIGHT_90]);
      		DeltaS += (int) (OAM_K_PS_45 * ps_value[PS_RIGHT_45]);
      		DeltaS += (int) (OAM_K_PS_00 * ps_value[PS_RIGHT_00]);
    	}
    	//cout << "delta1 :" << DeltaS << endl;
  	}
  
  	avoid_block (position);
  	if (!bloque && blocked < 500) {
  		if (fabs(angle) < 0.1 * M_PI) {
  			demi_tour = false;
  			DeltaS -= (int)(2 * (float)OAM_FORWARD_SPEED / M_PI * -angle);
  		}
  		else if (demi_tour || fabs(angle) > 0.9 * M_PI) {
  			demi_tour = true;
  			//cout << "demi-tour : "<< angle << endl;
  			oam_speed[LEFT] = 0;
		    oam_speed[RIGHT] = 0;
		    DeltaS += (angle > 0?1:-1) * OAM_K_MAX_DELTAS;
  		}
  		else {
  			DeltaS -= (int)(2 * (float)OAM_FORWARD_SPEED / M_PI * -angle);
  		}
  		//cout << "delta2 :" << DeltaS << endl;
	}	
	if (DeltaS > OAM_K_MAX_DELTAS) DeltaS = OAM_K_MAX_DELTAS;
    if (DeltaS < -OAM_K_MAX_DELTAS) DeltaS = -OAM_K_MAX_DELTAS;

    // Set speeds
    oam_speed[LEFT] -= DeltaS;
    oam_speed[RIGHT] += DeltaS;
	//cout << "speed: " << oam_speed[LEFT] << " " << oam_speed[RIGHT] << endl; 
  	left_speed = oam_speed[LEFT];
  	right_speed = oam_speed[RIGHT];
  	return bloque;
}

int ObstacleAvoidance::dist_sensor_get () const 
{ 
	return (int)ps_[2]->getValue (); 
}

int ObstacleAvoidance::analyse_cross_road (bool& left, bool& straight, bool& right)
{
	// fonction adaptée au laby de tolman. 
	
	// on analyse les alentours du robot (dvt, gauche, droite)
	// le test > 0 est pour supprimer l'artefact de debut de simulation
	if (obstacle_left_mid_get () > 0 && obstacle_left_mid_get () < 500) {
		left_near_ = true;
	} 
	else if (obstacle_left_mid_get () > 2000) {
		left_near_ = false;
	}		
	if (obstacle_right_mid_get () > 0 && obstacle_right_mid_get () < 500) {
		right_near_ = true;
	}
	else if (obstacle_right_mid_get () > 2000) {
		right_near_ = false;
	}
	bool left_reached = false;
	if (obstacle_left_get () > 0 && obstacle_left_get ()  < 500) {
		left_reached = true;
	}
	bool right_reached = false;
	if (obstacle_right_get () > 0 && obstacle_right_get ()  < 500) {
		right_reached = true;
	}
	bool front_reached = false;
	//if (robot_.obstacle_left_front_get() > 2000 && robot_.obstacle_right_front_get()) {
	if (obstacle_left_front_get() > 1500 && obstacle_right_front_get()) {
		front_reached = true;
	}
	//cout << obstacle_left_front_get() << endl;

	left = straight = right = false;
	// Quel type d'intersection ?
	if (left_near_ && left_reached) {
		if (right_near_) {
			if (right_reached) {
				if (!front_reached) {
					// on a 3 chemins (g,d,td)
					//cout << "on a 3 chemins (g,d,td)" << endl;
					left = straight = right = true;
					return 3;
				}
				else {
					// on a 2 chemins (g,d)
					//cout << "on a 2 chemins (g,d)" << endl;
					left = right = true;
					return 2;
				}
			}
			else {
				// il faut attendre encore le chemin d
				//cout << "il faut attendre encore le chemin d" << endl;
				return 1;
			}
		}
		else if (!front_reached) {
			// on a 2 chemins (g,td)
			//cout << "on a 2 chemins (g,td)" << endl;
			left = straight = true;
			return 2;
		}
		else {
			// on a 1 seul chemin, pas de décision
			//cout << "on a 1 seul chemin (g)" << endl;
			// on reinitialise la detection d'intersection
			left_near_ = false;
			return 1;	
		}
	}
	else if (right_near_ && right_reached) {
		if (left_near_ && !left_reached) {
			// il faut attendre encore le chemin g
			//cout << "il faut attendre encore le chemin g" << endl;
			return 1;
		}
		else if (!front_reached) {
			// on a 2 chemins (d,td)
			//cout << "on a 2 chemins (d,td)" << endl;
			right = straight = true;
			return 2;
		}
		else {
			// on a 1 seul chemin, pas de décision
			//cout << "on a 1 seul chemin (d)" << endl;
			// on reinitialise la detection d'intersection
			right_near_ = false;
			return 1;
		}
	}
	else if (!front_reached) {
			// on a 1 chemin (td)
			//cout << "on a 1 chemin (td)" << endl;
			straight = true;
			return 1;
	}
	return 0;	
} 

//int ObstacleAvoidance::analyse_cross_road_bis ()
//{
//	// fonction adaptée au laby de tolman. 
//	
//	// on analyse les alentours du robot
//	// le test > 0 est pour supprimer l'artefact de debut de simulation
//	bool left_reached = false;
//	if (obstacle_left_get () > 0 && obstacle_left_get ()  < 500) {
//		left_reached = true;
//	}
//	bool right_reached = false;
//	if (obstacle_right_get () > 0 && obstacle_right_get ()  < 500) {
//		right_reached = true;
//	}
//	bool front_reached = false;
//	if (obstacle_left_front_get() > 1500 && obstacle_right_front_get()) {
//		front_reached = true;
//	}
//	
//	int count = 0;
//	count = left_reached ? count + 1: count;
//	count = right_reached ? count + 1: count;
//	count = !front_reached ? count + 1: count;
//	cout << "count :" << count << endl;
//	return count;	
//} 

void ObstacleAvoidance::free_ways (vector<double>& dirs, double robot_angle)
{
	// à modifier pour que plus généralement, elle serve à découvrir
	// les directions empruntables par le robot selon un échantillonnage
	// ici on regarde tous les 90° (gauche, tout droit, droite et arrière)
	// On tient compte du demi-tour
	bool go_left = false, go_straight = false, go_right = false;
	analyse_cross_road (go_left, go_straight, go_right);
//	dirs.push_back (pi_pi (robot_angle - M_PI));
	if (go_left) {
		dirs.push_back (pi_pi (robot_angle + M_PI/2.0));
	}
	if (go_right) {
		dirs.push_back (pi_pi (robot_angle - M_PI/2.0));
	}
	if (go_straight) {
		dirs.push_back (pi_pi (robot_angle));
	}
}
