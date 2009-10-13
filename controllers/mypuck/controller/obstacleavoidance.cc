#include "obstacleavoidance.hh"
#include "params.hh"
#include "coord.hh"
#include "math.hh"
#include <cmath>
#include <iostream>

// OAM - Obstacle Avoidance Module (from e-puck_line controller)
//
// The OAM routine first detects obstacles in front of the robot, then records
// their side in "oam_side" and avoid the detected obstacle by 
// turning away according to very simple weighted connections between
// proximity sensors and motors. Output speeds are in oam_speed[LEFT] 
// and oam_speed[RIGHT].

// IR proximity sensors
enum sensors { PS_RIGHT_00 = 0, PS_RIGHT_45 = 1, PS_RIGHT_90 = 2, PS_RIGHT_REAR = 3,
	PS_LEFT_REAR = 4, PS_LEFT_90 = 5, PS_LEFT_45 = 6, PS_LEFT_00 = 7 };

// useful constants
enum directions { LEFT = 0, RIGHT = 1 };
#define OAM_OBST_THRESHOLD 3000
#define OAM_FORWARD_SPEED 400
#define OAM_K_PS_90 0.02
#define OAM_K_PS_45 0.09
#define OAM_K_PS_00 0.12
#define OAM_K_MAX_DELTAS 600
#define OAM_BLOQUE 8000

ObstacleAvoidance::ObstacleAvoidance (int* value):
	ps_value(value), left_near_(false), right_near_(false), bloque_(false), demi_tour(false), 
	blocked(0), last_x(0), last_y(0)
{
}

ObstacleAvoidance::~ObstacleAvoidance ()
{
}

void ObstacleAvoidance::avoid_block (const Coord& position) 
{
  	double x = position.x_get ();
	double y = position.y_get ();
	// Attention ce code est dépendant de TIME_STEP = 15ms
	// puisqu'il mesure l'avancée depuis le dernier step 
  	if (fabs (last_x - x) < 0.00025 && fabs (last_y - y) < 0.00025) {
    	blocked++;
  	}
  	else {
    	blocked = blocked > 0 ? blocked - 1: 0;
  	}
  	last_x = x;
  	last_y = y;
}

void ObstacleAvoidance::avoid (double angle, const Coord& position, int& left_speed, int& right_speed)
{
	int max_ds_value = 0, DeltaS = 0;
	int Activation[] = {0, 0};
	bool oam_active;
	int oam_speed[2];
		
	// Determine the presence and the side of an obstacle
	for (int i = PS_RIGHT_00; i <= PS_RIGHT_45; i++) {
		if (max_ds_value < ps_value[i]) max_ds_value = ps_value[i];
		Activation[RIGHT] += ps_value[i];
	}
	Activation[RIGHT] += ps_value[PS_RIGHT_90];
	for (int i = PS_LEFT_45; i <= PS_LEFT_00; i++) {
		if (max_ds_value < ps_value[i]) max_ds_value = ps_value[i];
		Activation[LEFT] += ps_value[i];
	}
	Activation[LEFT] += ps_value[PS_LEFT_90];
	if (max_ds_value > OAM_OBST_THRESHOLD) {
		oam_active = true;
	}
	else {
		oam_active = false;
		bloque_ = false;
	}
//	cout << endl << Activation[LEFT] << " " << Activation[RIGHT] << " " << OAM_OBST_THRESHOLD << " " << blocked << endl;

	if (oam_active && !bloque_) {
 		if (Activation[RIGHT] > OAM_BLOQUE && Activation[LEFT] > OAM_BLOQUE) {
			bloque_ = true;
  			//cout << "bloque" << endl;
		}
	}

	// Forward speed
	oam_speed[LEFT]  = OAM_FORWARD_SPEED;
	oam_speed[RIGHT] = OAM_FORWARD_SPEED;

	// Go away from obstacle
  	if (!demi_tour && oam_active) {
    	// The rotation of the robot is determined by the location and the side of the obstacle
    	if (Activation[RIGHT] <= Activation[LEFT]) {
    		DeltaS -= (int) (OAM_K_PS_90 * ps_value[PS_LEFT_90]);
      		DeltaS -= (int) (OAM_K_PS_45 * ps_value[PS_LEFT_45]);
      		DeltaS -= (int) (OAM_K_PS_00 * ps_value[PS_LEFT_00]);
    	}
    	else { 
    		DeltaS += (int) (OAM_K_PS_90 * ps_value[PS_RIGHT_90]);
      		DeltaS += (int) (OAM_K_PS_45 * ps_value[PS_RIGHT_45]);
      		DeltaS += (int) (OAM_K_PS_00 * ps_value[PS_RIGHT_00]);
    	}
    	//cout << "delta1 :" << DeltaS << endl;
  	}
  
  	avoid_block (position);
//  	cout << "blocked " << blocked << "bloc " << bloque_ << " demi_tour " << demi_tour << " angle " << angle << endl;
  	if (!bloque_ && blocked < 500) {
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
}

int ObstacleAvoidance::analyse_cross_road (bool& left, bool& straight, bool& right)
{
	// fonction adaptée au laby de tolman. 
	
	// on analyse les alentours du robot (dvt, gauche, droite)
	if (ps_value[PS_LEFT_45] < 200) {
		left_near_ = true;
	} 
	else if (ps_value[PS_LEFT_45] > 1700) {
		left_near_ = false;
	}		
	if (ps_value[PS_RIGHT_45] < 200) {
		right_near_ = true;
	}
	else if (ps_value[PS_RIGHT_45] > 1700) {
		right_near_ = false;
	}
	bool left_reached = false;
	if (ps_value[PS_LEFT_90]  < 200) {
		left_reached = true;
	}
	bool right_reached = false;
	if (ps_value[PS_RIGHT_90]  < 200) {
		right_reached = true;
	}
	bool front_reached = false;
	//if (ps_value[PS_LEFT_00] > 1700) {
	if (ps_value[PS_LEFT_00] > 1200) {
		front_reached = true;
	}
	//cout << obstacle_left_front << endl;

	left = straight = right = false;
	// Quel type d'intersection ?
	if (left_near_ && left_reached) {
		if (right_near_) {
			if (right_reached) {
				if (!front_reached) {
					// on a 3 chemins (g,d,td)
//					cout << "on a 3 chemins (g,d,td)" << endl;
					left = straight = right = true;
					return 3;
				}
				else {
					// on a 2 chemins (g,d)
//					cout << "on a 2 chemins (g,d)" << endl;
					left = right = true;
					return 2;
				}
			}
			else {
				// il faut attendre encore le chemin d
//				cout << "il faut attendre encore le chemin d" << endl;
				return 1;
			}
		}
		else if (!front_reached) {
			// on a 2 chemins (g,td)
//			cout << "on a 2 chemins (g,td)" << endl;
			left = straight = true;
			return 2;
		}
		else {
			// on a 1 seul chemin, pas de décision
//			cout << "on a 1 seul chemin (g)" << endl;
			// on reinitialise la detection d'intersection
			left_near_ = false;
			return 1;	
		}
	}
	else if (right_near_ && right_reached) {
		if (left_near_ && !left_reached) {
			// il faut attendre encore le chemin g
//			cout << "il faut attendre encore le chemin g" << endl;
			return 1;
		}
		else if (!front_reached) {
			// on a 2 chemins (d,td)
//			cout << "on a 2 chemins (d,td)" << endl;
			right = straight = true;
			return 2;
		}
		else {
			// on a 1 seul chemin, pas de décision
//			cout << "on a 1 seul chemin (d)" << endl;
			// on reinitialise la detection d'intersection
			right_near_ = false;
			return 1;
		}
	}
	else if (!front_reached) {
			// on a 1 chemin (td)
//			cout << "on a 1 chemin (td)" << endl;
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
//	if (obstacle_left_get () > 0 && obstacle_left_get ()  < 200) {
//		left_reached = true;
//	}
//	bool right_reached = false;
//	if (obstacle_right_get () > 0 && obstacle_right_get ()  < 200) {
//		right_reached = true;
//	}
//	bool front_reached = false;
//	if (obstacle_left_front_get() > 1300) {
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
