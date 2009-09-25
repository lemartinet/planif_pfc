#ifndef ACTION_HH
# define ACTION_HH

class Action
{
public:  
	Action (double angle);
	Action (double angle, int speed);

	inline
  	Action* clone () { return new Action (angle_, speed_); }
  
  	bool equal (Action& action);

  	inline
  	int speed_get () { return speed_; }
  
  	inline
  	void speed_set (int speed) { speed_ = speed; }
  
  	inline
  	double angle_get () { return angle_; }
  	
  	inline
  	void angle_set (double angle) { angle_ = angle; }

protected:
  	int speed_;
  	double angle_;
};

bool operator== (Action& a1, Action& a2);

#endif
