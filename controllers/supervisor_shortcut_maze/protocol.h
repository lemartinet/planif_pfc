#ifndef PROTOCOL_H_
#define PROTOCOL_H_

void init_protocol(int simu_id, int shortcut);
void run_protocol();
void die_protocol();
int get_trial();
int get_total_step();
double get_trial_time();

#endif /*PROTOCOL_H_*/
