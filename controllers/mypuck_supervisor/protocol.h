#ifndef PROTOCOL_H_
#define PROTOCOL_H_

<<<<<<< HEAD
void init_protocol ();
=======
void init_protocol (int simu_id, int final_forced);
>>>>>>> 16cde28... First commit to git. Includes:
void do_orders ();
int go_way1 ();
void move_doors ();
void run_protocol (int* day, int* trial, double* trial_time, int* total_step_);
int learning_done ();
void open_all ();
void die_protocol (int* day, int* trial);
void sleep_step_set (int step);
<<<<<<< HEAD
=======
void message_protocol (const char* s, int total_step, int day, int trial);
>>>>>>> 16cde28... First commit to git. Includes:

#endif /*PROTOCOL_H_*/
