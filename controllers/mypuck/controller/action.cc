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
  		
bool Action::equals (const Action& a2) const
{
	static const double ANGLE_THRESH = params->get_double("ANGLE_THRESH");
	if (fabs (ecart_angulaire (angle_, a2.angle_get ())) < ANGLE_THRESH) {
		return true;
	}
	else {
  		return false;
	}
}

bool operator== (const Action& a1, const Action& a2)
{
  return a1.equals (a2);
}
