#ifndef ANALYSEBEHAVIOR_H_
#define ANALYSEBEHAVIOR_H_

enum message_type { WAY1 = 1, _WAY1 = -1,
                    WAY2 = 2, _WAY2 = -2, 
                    WAY3 = 3, _WAY3 = -3, 
                    WAY4 = 4, _WAY4 = -4, 
                    GOAL = 5, EXCEED = 6, SLEEP = 7 };
int first_way;
int current_way;

void init_behavior (int simu_id, int write_step);
void run_behavior ();
void die_behavior ();
void message (enum message_type s);

#endif /*ANALYSEBEHAVIOR_H_*/
