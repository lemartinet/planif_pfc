#include <iostream>
#include <cmath>
#include "action.hh"
#include "math.hh"
#include "params.hh"

Action::Action (double angle) : angle_(angle) 
{
	// le static const permet d'initialiser une seule fois la variable !
	static const int DEFAULT_SPEED = Params::get_int("DEFAULT_SPEED");
	speed_ = DEFAULT_SPEED;
}

Action::Action (double angle, int speed) : speed_(speed), angle_(angle) {}
  		
bool operator== (const Action& a1, const Action& a2)
{
  return angle_equal (a1.angle_get (), a2.angle_get ());
}
