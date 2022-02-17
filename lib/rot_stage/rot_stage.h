#ifndef ROT_STAGE_H
#define ROT_STAGE_H

// calibration
#define PULSE_COEF 398L
#define HEX_LENGTH (8+1)

// Pins
#define JOG D1
#define FWD D2
#define BWD D3
#define MOT D0
#define PD A0
// Trigger
#ifndef TRIG
#define TRIG D6
#endif // TRIG

// EEPROM
#define MIN_EEPROM_ADDRESS 0
#define MAX_EEPROM_ADDRESS 4

#include <Arduino.h>


void init_rot_stage();
String send_cmd(String cmd);
void cancel_cmd();
// Movement
void move_home(bool dir);
void move_absolute(float angle);
void move_relative(float angle);
void move_fwd();
void move_bwd();
void move_min();
void move_max();
// Settings
void set_jog_step(float step);
void set_speed(uint8_t v);
void set_angle_time(float angle, float t);
void set_min(float angle_min);
void set_max(float angle_max);
void set_calibration_ranges(int angle_min, int angle_max);
void calibration_min_max();

// Jog mode
void jog_fwd();
void jog_bwd();

// External interrupt
void ICACHE_RAM_ATTR handle_trig_interrupt();

#endif // ROT_STAGE_H