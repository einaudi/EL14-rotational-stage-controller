#ifndef SERVER_RS_H
#define SERVER_RS_H

#ifndef LED
#define LED D4
#endif //LED

// Server
void init_server();
void serve();
void handle_OnConnect();
void handle_NotFound();

// Rotational stage handling
// Move
void handle_home();
void handle_move_absolute();
void handle_move_relative();
void handle_move_power();
void handle_move_fwd();
void handle_move_bwd();
void handle_move_min();
void handle_move_max();
// Settings
void handle_set_speed();
void handle_set_jog_step();
void handle_set_angle_time();
void handle_set_min_max();
void handle_set_calibration_ranges();
void handle_calibration_min_max();
void handle_set_trigger_angles();
void handle_set_trigger_power();
// Cicero handler
void handle_getdata();
void handle_start();
void handle_addproperty();
void handle_end();

// Misc
void blink(int n);
void send_ip();

#endif // SERVER_RS_H