#ifndef PROTOCOL_H_
#define PROTOCOL_H_

void init_protocol ();
void do_orders ();
int go_way1 ();
void move_doors ();
void run_protocol (int* day, int* trial, double* trial_time, int* total_step_);
int learning_done ();
void open_all ();
void die_protocol (int* day, int* trial);
void sleep_step_set (int step);

#endif /*PROTOCOL_H_*/
