#include <iostream>
#include <cmath>
#include "action.hh"
#include "math.hh"
#include "params.hh"

Action::Action (double angle) : angle_(angle) {}
  		
bool operator== (const Action& a1, const Action& a2)
{
  return angle_equal (a1.angle_get (), a2.angle_get ());
}
