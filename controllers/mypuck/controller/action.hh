#ifndef ACTION_HH
# define ACTION_HH

class Action
{
public:  
	Action (double angle);

	double angle_get () const { return angle_; }
	void angle_set (double angle) { angle_ = angle; }

protected:
  	double angle_;
};

bool operator== (const Action& a1, const Action& a2);

#endif
