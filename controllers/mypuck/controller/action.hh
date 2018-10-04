#ifndef ACTION_HH
# define ACTION_HH

class Action
{
public:  
	Action (double angle);
	Action (double angle, int speed);

  	bool equals (const Action& action) const;
  	int speed_get () const { return speed_; }  
  	void speed_set (int speed) { speed_ = speed; }
	double angle_get () const { return angle_; }
	void angle_set (double angle) { angle_ = angle; }

protected:
  	int speed_;
  	double angle_;
};

bool operator== (const Action& a1, const Action& a2);

#endif
