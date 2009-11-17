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
// proximity sensors and motors.

// IR proximity sensors
enum sensors { PS_RIGHT_00 = 0, PS_RIGHT_45 = 1, PS_RIGHT_90 = 2, PS_RIGHT_REAR = 3,
	PS_LEFT_REAR = 4, PS_LEFT_90 = 5, PS_LEFT_45 = 6, PS_LEFT_00 = 7 };
// directions
enum directions { LEFT = 0, RIGHT = 1 };

ObstacleAvoidance::ObstacleAvoidance (int* value):
	ps_value(value), left_near_(false), right_near_(false), oam_state_(OAM_OFF) {}

ObstacleAvoidance::~ObstacleAvoidance ()
{
}

void ObstacleAvoidance::avoid (double angle, int& left_speed, int& right_speed)
{
	static const int OAM_OBST_THRESHOLD = 4500;
	static const double OAM_K_PS_90 = 0.02;
	static const double OAM_K_PS_45 = 0.09;
	static const double OAM_K_PS_00 = 0.12;

	// Determine the presence and the side of an obstacle
	int Activation[] = {0, 0};
	for (int i = PS_RIGHT_00; i <= PS_RIGHT_45; i++) {
		Activation[RIGHT] += ps_value[i];
	}
	for (int i = PS_LEFT_45; i <= PS_LEFT_00; i++) {
		Activation[LEFT] += ps_value[i];
	}
//	cout << " " << Activation[LEFT] << " " << Activation[RIGHT] << endl;

	// update de l'etat de l'oam
	if (Activation[RIGHT] > OAM_OBST_THRESHOLD && Activation[LEFT] > OAM_OBST_THRESHOLD) {
		oam_state_ = OAM_STUCK;
//		cout << "bloque" << endl;
	}
	else if (Activation[LEFT] > OAM_OBST_THRESHOLD || Activation[RIGHT] > OAM_OBST_THRESHOLD) {
		oam_state_ = OAM_ON;
//		cout << "oam_active" << endl;
	}
	else if (fabs(angle) > 0.9 * M_PI) {
		oam_state_ = OAM_BIGTURN;
//		cout << "demi-tour : "<< angle << endl;
	}
	else if (fabs(angle) < 0.1 * M_PI) {
		oam_state_ = OAM_OFF;
	}

	// calcul des vitesses de roues en fonction de l'etat de l'oam
	static const int OAM_FORWARD_SPEED = Params::get_int("DEFAULT_SPEED");
	int DeltaS = 0;
	left_speed = right_speed = OAM_FORWARD_SPEED;
  	if (oam_state_ == OAM_ON) {
  		// on ralentit pas mais on tourne selon la proximité du mur
  		bool go_right = Activation[RIGHT] <= Activation[LEFT];
		DeltaS += (go_right ? -1 : 1) * OAM_K_PS_90 * ps_value[(go_right ? PS_LEFT_90 : PS_RIGHT_90)];
		DeltaS += (go_right ? -1 : 1) * OAM_K_PS_45 * ps_value[(go_right ? PS_LEFT_45 : PS_RIGHT_45)];
		DeltaS += (go_right ? -1 : 1) * OAM_K_PS_00 * ps_value[(go_right ? PS_LEFT_00 : PS_RIGHT_00)];
  	}
  	else if (oam_state_ == OAM_BIGTURN || oam_state_ == OAM_STUCK) {
  		// on freine et on tourne à fond
  		left_speed = right_speed = 0;
		DeltaS += (angle > 0 ? 1 : -1) * OAM_FORWARD_SPEED;
	}
	else {
		// on ralentit lineairement et on tourne exponentiellement suivant la force du virage
  		left_speed = right_speed = OAM_FORWARD_SPEED * (1 - fabs(angle) / M_PI);
		DeltaS += OAM_FORWARD_SPEED * (angle > 0 ? 1 : -1) * (1 - exp(-fabs(1.5 * angle)));
	}
	if (DeltaS > OAM_FORWARD_SPEED) DeltaS = OAM_FORWARD_SPEED;
    if (DeltaS < -OAM_FORWARD_SPEED) DeltaS = -OAM_FORWARD_SPEED;

    // Set speeds
    left_speed -= DeltaS;
    right_speed += DeltaS;
	//cout << "speed: " << left_speed << " " << right_speed << endl;
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
	if (ps_value[PS_LEFT_00] > 1200 && ps_value[PS_RIGHT_00] > 1200) {
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
