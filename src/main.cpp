#include <Arduino.h>

#include "rot_stage.h"
#include "server_RS.h"

int dir = 1;
int steps_delay = 10;

void setup() {
  init_rot_stage();
  init_server();

  /*
  set_jog_step(1);
  set_speed(100);
  // delay(1000);
  move_absolute(70);
  delay(1000);
  */

  blink(2);
}

void loop() {
  serve();

  /*
  if(digitalRead(TRIG)) {
    move_max();
  }
  else {
    move_min();
  }
  */
  /*
  for(int i=0; i<90; i++) {
    move_relative(dir*10);
    delay(10);
  }
  Serial.print("0gs\n");
  String ret = Serial.readStringUntil('\n');
  dir *= -1;
  delay(1000);
  */

  /*
  digitalWrite(TRIG, HIGH);
  for(int i=0; i<20; i++) {
    jog_fwd();
    delay(1);
    while(!digitalRead(MOT)) {}
    // delay(steps_delay);
  }
  digitalWrite(TRIG, LOW);
  for(int i=0; i<20; i++) {
    jog_bwd();
    delay(1);
    while(!digitalRead(MOT)) {}
    // delay(steps_delay);
  }
  */
  

  /*
  // move_home(1);
  move_absolute(70);
  delay(1000);
  digitalWrite(TRIG, HIGH);
  for(int i=0; i<12; i++) {
    move_relative(1);
    delay(steps_delay);
    // cancel_cmd();
    // while(!digitalRead(MOT)) {}
  }
  digitalWrite(TRIG, LOW);
  delay(1000);
  */
}