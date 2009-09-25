#include <iostream>
#include <cmath>
#include "action.hh"
#include "math.hh"
#include "params.hh"

extern Params* params;

Action::Action (double angle) : angle_(angle) 
{
	// le static const permet d'initialiser une seule fois la variable !
	static const int DEFAULT_SPEED = params->get_int("DEFAULT_SPEED");
	speed_ = DEFAULT_SPEED;
}

Action::Action (double angle, int speed) : speed_(speed), angle_(angle) {}
  		
bool Action::equal (Action& a2)
{
	static const double ANGLES_EQUALITY_THRESH = params->get_double("ANGLES_EQUALITY_THRESH");
	if (fabs (ecart_angulaire (angle_, a2.angle_get ())) < ANGLES_EQUALITY_THRESH) {
		return true;
	}
	else {
  		return false;
	}
}

bool operator== (Action& a1, Action& a2)
{
  return a1.equal (a2);
}
