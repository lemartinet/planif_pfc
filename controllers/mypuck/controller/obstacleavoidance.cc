#include "obstacleavoidance.hh"
#include "params.hh"
#include "coord.hh"
#include "math.hh"
#include <cmath>
#include <iostream>
#include <fstream>

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

ObstacleAvoidance::ObstacleAvoidance (int* value): ps_value(value), oam_state_(OAM_OFF)
{
}

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
  		left_speed = right_speed = OAM_FORWARD_SPEED / 4;
		DeltaS += (angle > 0 ? 1 : -1) * OAM_FORWARD_SPEED;
	}
	else {
		// on ralentit lineairement et on tourne exponentiellement
		// suivant la force du virage
//		cout << "turning..." << endl;
  		left_speed = right_speed = OAM_FORWARD_SPEED * (1 - fabs(angle) / (2 * M_PI));
//		DeltaS += OAM_FORWARD_SPEED * (angle > 0 ? 1 : -1) * (1 - exp(-fabs(1.5 * angle) / 2));
  		DeltaS += OAM_FORWARD_SPEED * (angle > 0 ? 1 : -1) * fabs(angle) / M_PI;
	}
	if (DeltaS > OAM_FORWARD_SPEED) DeltaS = OAM_FORWARD_SPEED;
    if (DeltaS < -OAM_FORWARD_SPEED) DeltaS = -OAM_FORWARD_SPEED;

    // Set speeds
    left_speed -= DeltaS;
    right_speed += DeltaS;
	//cout << "speed: " << left_speed << " " << right_speed << endl;
}

void ObstacleAvoidance::free_ways (vector<double>& dirs, double robot_angle)
{
	// NOTE : on ne détecte pas les chemins à 45°, à changer dans le code en dessous


//	cout << "---" << endl;
//	cout << ps_value[PS_LEFT_45] << " " << ps_value[PS_RIGHT_45] << endl;
//	cout << ps_value[PS_LEFT_90] << " " << ps_value[PS_RIGHT_90] << endl;

	if (ps_value[PS_LEFT_00] + ps_value[PS_RIGHT_00] < 1500) {
//		cout << "dir 0 free" << endl;
		dirs.push_back(pi_pi(robot_angle));
	}
	// On tient compte du demi-tour
//	dirs.push_back (pi_pi (robot_angle - M_PI));


//	// Méthodes avec les capteurs 45 et 90
//	// on divise l'espace des ps (PS_RIGHT_45, PS_RIGHT_90) en deux zones
//	if (ps_value[PS_RIGHT_45] + ps_value[PS_RIGHT_90] < 4000) {
//		// zone avec quelque chose à droite. On redivise cette zone en deux
//		if (ps_value[PS_RIGHT_45] > 500 && ps_value[PS_RIGHT_90] > 2500) {
//			cout << "dir right 90 free" << endl;
//			dirs.push_back(pi_pi(robot_angle - M_PI/2.0));
//		} else if (ps_value[PS_RIGHT_45] < 500 && ps_value[PS_RIGHT_90] < 2500) {
//			cout << "dir right 45 free" << endl;
//			dirs.push_back(pi_pi(robot_angle - M_PI/4.0));
//		}
//	}
//	if (ps_value[PS_LEFT_45] + ps_value[PS_LEFT_90] < 4000) {
//		// quelque chose à gauche !
//		if (ps_value[PS_LEFT_45] > 500 && ps_value[PS_LEFT_90] > 2500) {
//			cout << "dir left 90 free" << endl;
//			dirs.push_back(pi_pi(robot_angle + M_PI/2.0));
//		} else if (ps_value[PS_LEFT_45] < 500 && ps_value[PS_LEFT_90] < 2500) {
//			cout << "dir left 45 free" << endl;
//			dirs.push_back(pi_pi(robot_angle + M_PI/4.0));
//		}
//	}

	if (ps_value[PS_RIGHT_45]  < 1500) {
		// zone avec quelque chose à droite. On redivise cette zone en deux
		if (ps_value[PS_RIGHT_45] < 500) {
//			cout << "dir right 45/90 free" << endl;
//			dirs.push_back(pi_pi(robot_angle - M_PI/4.0));
			dirs.push_back(pi_pi(robot_angle - M_PI/2.0));
		}
		else if (ps_value[PS_RIGHT_45] < 1500) {
//			cout << "dir right 90 free" << endl;
			dirs.push_back(pi_pi(robot_angle - M_PI/2.0));
		}
	}
	if (ps_value[PS_LEFT_45]  < 1500) {
		// zone avec quelque chose à droite. On redivise cette zone en deux
		if (ps_value[PS_LEFT_45] < 500) {
//			cout << "dir left 45/90 free" << endl;
//			dirs.push_back(pi_pi(robot_angle + M_PI/4.0));
			dirs.push_back(pi_pi(robot_angle + M_PI/2.0));
		}
		else if (ps_value[PS_LEFT_45] < 1500) {
//			cout << "dir left 90 free" << endl;
			dirs.push_back(pi_pi(robot_angle + M_PI/2.0));
		}
	}
}
