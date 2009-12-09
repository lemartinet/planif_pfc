#ifndef ANALYSEBEHAVIOR_H_
#define ANALYSEBEHAVIOR_H_

enum message_type { WAY1 = 1, _WAY1 = -1,
                    WAY2 = 2, _WAY2 = -2, 
                    GOAL = 5, EXCEED = 6};

void init_behavior (int simu_id, int write_step);
void run_behavior ();
void die_behavior ();
void message (enum message_type s);
void current_way_init ();

#endif /*ANALYSEBEHAVIOR_H_*/
