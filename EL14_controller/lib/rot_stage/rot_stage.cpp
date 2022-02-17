#include <Arduino.h>
#include <EEPROM.h>
#include "rot_stage.h"


float _angle_min = 0;
float _angle_max = 45;

int _cal_lim_min = 0;
int _cal_lim_max = 120;

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

    // Min/max values
    EEPROM.begin(8);
    delay(10);
    EEPROM.get(MIN_EEPROM_ADDRESS, _angle_min);
    EEPROM.get(MAX_EEPROM_ADDRESS, _angle_max);

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
    move_home(1);
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

// Settings
void set_jog_step(float step) {
    int32_t angle_p = step * PULSE_COEF;
    char cmd[11];

    sprintf(cmd, "0sj%08X", angle_p);

    send_cmd(cmd);
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
    move_min();
}