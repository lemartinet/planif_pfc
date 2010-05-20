#ifndef PROTOCOL_H_
#define PROTOCOL_H_

int sleep_freq;
enum protocol_type { P1 = 1, P2 = 2, P3 = 3, FREE = 4, BLOCA = 5, BLOCB = 6 } protocol_state;

void init_protocol(int simu_id, int sleep);
void run_protocol();
void die_protocol();
void open_all();
int get_trial();
int get_day();
int get_total_step();
double get_trial_time();

#endif /*PROTOCOL_H_*/
