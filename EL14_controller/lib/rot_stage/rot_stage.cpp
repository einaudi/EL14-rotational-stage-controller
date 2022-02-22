#include <Arduino.h>
#include <EEPROM.h>
#include "rot_stage.h"


float _angle_min = 0.;
float _angle_max = 45.;

float _angle_trig_start = 0.;
float _angle_trig_stop = 45.;
float _time_trig = 0.1;

float _jog_step = 45.;

int _cal_lim_min = 0;
int _cal_lim_max = 80;

void init_rot_stage() {
    // Pins
    pinMode(MOT, INPUT_PULLDOWN_16);
    pinMode(TRIG, INPUT);

    pinMode(FWD, OUTPUT);
    digitalWrite(FWD, HIGH);

    pinMode(BWD, OUTPUT);
    digitalWrite(BWD, HIGH);
    
    pinMode(JOG, OUTPUT);
    digitalWrite(JOG, HIGH);

    // EEPROM readout
    EEPROM.begin(24);
    delay(10);
    EEPROM.get(MIN_EEPROM_ADDRESS, _angle_min);
    EEPROM.get(MAX_EEPROM_ADDRESS, _angle_max);
    EEPROM.get(TRIG_START_EEPROM_ADDRESS, _angle_trig_start);
    EEPROM.get(TRIG_STOP_EEPROM_ADDRESS, _angle_trig_stop);
    EEPROM.get(TRIG_TIME_EEPROM_ADDRESS, _time_trig);
    EEPROM.get(JOG_STEP_EEPROM_ADDRESS, _jog_step);

    // Interrupt
    attachInterrupt(digitalPinToInterrupt(TRIG), handle_trig_interrupt, RISING);

    // Wait till EL14 finishes initialisation
    delay(5000);
    while(!digitalRead(MOT)) {}

    // Serial
    Serial.begin(9600);
    while(!Serial) {}
    Serial.swap();
    
    Serial.print("0gs\n");
    String ret = Serial.readStringUntil('\n');
    set_speed(100);
    move_min();
    set_trigger(_angle_trig_start, _angle_trig_stop, _time_trig);
}

String send_cmd(String cmd) {
    while(!digitalRead(MOT)) {}
    Serial.print(cmd);
    String ret = Serial.readStringUntil('\n');

    return ret;
}

void cancel_cmd() {
    while(!digitalRead(MOT)) {}
    Serial.print(0x0D);
    delay(1);
}

// Movement
void move_home(bool dir) {
    String cmd = dir ? "0ho1" : "0ho0";

    send_cmd(cmd);
}

void move_absolute(float angle){
    int32_t angle_p = angle * PULSE_COEF;
    char cmd[11];

    sprintf(cmd, "0ma%08X", angle_p);

    send_cmd(cmd);
}

void move_relative(float angle) {
    int32_t angle_p = angle * PULSE_COEF;
    char cmd[11];

    sprintf(cmd, "0mr%08X", angle_p);

    send_cmd(cmd);
}

void move_power(float power) {
    float angle = power_to_angle(power);
    move_absolute(angle);
}

void move_fwd() {
    send_cmd("0fw");
}

void move_bwd() {
    send_cmd("0bw");
}

void move_min() {
    move_absolute(_angle_min);
}

void move_max() {
    move_absolute(_angle_max);
}

void move_trigger_start() {
    move_absolute(_angle_trig_start);
}

void move_trigger_stop() {
    move_absolute(_angle_trig_stop);
}

// Settings
void set_jog_step(float step) {
    int32_t angle_p = step * PULSE_COEF;
    char cmd[11];

    sprintf(cmd, "0sj%08X", angle_p);

    send_cmd(cmd);

    _jog_step = step;
    EEPROM.put(JOG_STEP_EEPROM_ADDRESS, _jog_step);
    EEPROM.commit();
}

void set_speed(uint8_t v) {
    char cmd[5];
    v = v > 100 ? 100 : v;
    v = v < 1 ? 1 : v;
    sprintf(cmd, "0sv%02X", v);

    send_cmd(cmd);
}

void set_angle_time(float angle, float t) {
    float v_d = float(angle)/float(t);
    // Workaround to match calibration
    v_d = (v_d - 13.67)/0.826;

    float v_p = -4.7929;
    v_p += 3.0265e-1 * v_d;
    v_p += -6.7971e-4 * v_d * v_d;
    v_p += 7.3075e-7 * v_d * v_d * v_d;

    set_speed((uint8_t) v_p);
}

void set_min(float angle_min) {
    _angle_min = angle_min;
    EEPROM.put(MIN_EEPROM_ADDRESS, _angle_min);
    EEPROM.commit();
}

void set_max(float angle_max) {
    _angle_max = angle_max;
    EEPROM.put(MAX_EEPROM_ADDRESS, _angle_max);
    EEPROM.commit();
}

void set_calibration_ranges(int angle_min, int angle_max) {
    _cal_lim_min = angle_min;
    _cal_lim_max = angle_max;
}

void calibration_min_max() {
    int i = _cal_lim_min;
    int value_min = 1023;
    int value_max = 0;
    float angle_min;
    float angle_max;
    for(; i < _cal_lim_max; i++) {
        // Rotate to next step
        move_absolute((float)i);
        // Measure PD input
        
        int value = 0;
        for(int j=0; j<10; j++) {
            value += analogRead(PD);
            delay(1);
        }
        value /= 10;
        
        // Update values
        if(value < value_min) {
            value_min = value;
            angle_min = i;
        }
        if(value > value_max) {
            value_max = value;
            angle_max = i;
        }
        delay(10);
    }
    set_min(angle_min);
    set_max(angle_max);
}

void set_trigger_start(float angle_start) {
    _angle_trig_start = angle_start;
    EEPROM.put(TRIG_START_EEPROM_ADDRESS, _angle_trig_start);
    EEPROM.commit();
}

void set_trigger_stop(float angle_stop) {
    _angle_trig_stop = angle_stop;
    EEPROM.put(TRIG_STOP_EEPROM_ADDRESS, _angle_trig_stop);
    EEPROM.commit();
}

void set_trigger_time(float t) {
    _time_trig = t;
    EEPROM.put(TRIG_TIME_EEPROM_ADDRESS, _time_trig);
    EEPROM.commit();
}

void set_trigger(float angle_start, float angle_stop, float t) {
    // Parameters
    set_trigger_start(angle_start);
    set_trigger_stop(angle_stop);
    set_trigger_time(t);

    // Speed
    float angle_diff = angle_stop - angle_start;
    angle_diff = angle_diff < 0 ? -angle_diff : angle_diff;
    set_angle_time(angle_diff, t);
    set_jog_step(angle_diff);
}

float power_to_angle(float power) {
    if(power < 0) power = 0.;
    else if(power > 1) power = 1.;

    float ret = 5.9098e1;
    ret += -1.5665e2 * power;
    ret += 1.0376e3 * pow(power, 2.);
    ret += -4.5464e3 * pow(power, 3.);
    ret += 1.1062e4 * pow(power, 4.);
    ret += -1.4904e4 * pow(power, 5.);
    ret += 1.0395e4 * pow(power, 6.);
    ret += -2.9311e3 * pow(power, 7.);

    return ret;
}

// Jog mode
void jog_fwd() {
    while(!digitalRead(MOT)) {};
    digitalWrite(JOG, LOW);
    delayMicroseconds(10);
    digitalWrite(FWD, LOW);

    delayMicroseconds(100);

    digitalWrite(FWD, HIGH);
    digitalWrite(JOG, HIGH);

    String ret = Serial.readStringUntil('\n');
}

void jog_bwd() {
    while(!digitalRead(MOT)) {};
    digitalWrite(JOG, LOW);
    delayMicroseconds(10);
    digitalWrite(BWD, LOW);

    delayMicroseconds(100);

    digitalWrite(BWD, HIGH);
    digitalWrite(JOG, HIGH);

    String ret = Serial.readStringUntil('\n');
}

// External interrupt
void ICACHE_RAM_ATTR handle_trig_interrupt() {
    // Jog
    if(_angle_trig_start < _angle_trig_stop) {
        jog_fwd();
    }
    else {
        jog_bwd();
    }
    // Command
    // move_trigger_stop();
}