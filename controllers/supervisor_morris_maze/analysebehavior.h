#ifndef ANALYSEBEHAVIOR_H_
#define ANALYSEBEHAVIOR_H_

enum message_type { GOAL = 1, EXCEED = 2, SLEEP = 3 };

void init_behavior (int simu_id, int write_step);
void run_behavior ();
void die_behavior ();
void message (enum message_type s);

#endif /*ANALYSEBEHAVIOR_H_*/
