#ifndef PROTOCOL_H_
#define PROTOCOL_H_

void init_protocol(int simu_id, int final_forced, int sleep);
void run_protocol();
void die_protocol();
int learning_done();
void open_all();
int get_trial();
int get_day();
int get_total_step();
double get_trial_time();

#endif /*PROTOCOL_H_*/
