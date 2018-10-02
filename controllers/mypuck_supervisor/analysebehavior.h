#ifndef ANALYSEBEHAVIOR_H_
#define ANALYSEBEHAVIOR_H_

void analyse_behavior (int day, int trial, double trial_time, int total_step);
void die_behavior (int day, int trial);
void init_behavior (int simu_id);
void message (const char* s, int total_step, int day, int trial, double trial_time);
void new_trial_message (int trial);
void write_step_set (int step);
void emit_goal_found ();
void emit_sleep ();
void current_way_init ();

#endif /*ANALYSEBEHAVIOR_H_*/
