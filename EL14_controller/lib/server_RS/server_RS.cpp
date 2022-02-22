#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include "server_RS.h"
#include "rot_stage.h"


const String available_cmds[] = {
  "home",
  "move_absolute",
  "move_relative",
  "move_fwd",
  "move_bwd",
  "move_min",
  "move_max",
  "set_speed",
  "set_jog_step",
  "set_angle_time",
  "set_min_max",
  "set_calibration_ranges",
  "calibration_min_max",
  "set_trigger_angles",
  "set_trigger_power"
};

/* Put your SSID & Password */
const char* ssid = "Ultra_Cold";  // Enter SSID here
const char* password = "cold-centre-1";  //Enter Password here

// Server
ESP8266WebServer server(80);

void init_server() {
  // Pins
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  }
  send_ip();
  
  // Server paths
  server.on("/", handle_OnConnect);
  server.on("/test", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  // Move
  server.on("/home", handle_home);
  server.on("/move_absolute", handle_move_absolute);
  server.on("/move_relative", handle_move_relative);
  server.on("/move_power", handle_move_power);
  server.on("/move_fwd", handle_move_fwd);
  server.on("/move_bwd", handle_move_bwd);
  server.on("/move_min", handle_move_min);
  server.on("/move_max", handle_move_max);
  // Settings
  server.on("/set_speed", handle_set_speed);
  server.on("/set_jog_step", handle_set_jog_step);
  server.on("/set_angle_time", handle_set_angle_time);
  server.on("/set_min_max", handle_set_min_max);
  server.on("/set_calibration_ranges", handle_set_calibration_ranges);
  server.on("/calibration_min_max", handle_calibration_min_max);
  server.on("/set_trigger_angles", handle_set_trigger_angles);
  server.on("/set_trigger_power", handle_set_trigger_power);
  // Cicero handler
  server.on("/getdata", handle_getdata);
  server.on("/start", handle_start);
  server.on("/addproperty", handle_addproperty);
  server.on("/end", handle_end);
  
  server.begin();
}

void serve() {
  server.handleClient();
}

void handle_OnConnect() {
  // It is crucial to send OK - it's needed by handler
  server.send(200, "text/html", "OK"); 
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

// Move
void handle_home() {
  move_home((bool)server.arg(0).toInt());
  /*
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"dir") {
      move_home((bool)server.arg(i).toInt());
    }
  }
  */
  server.send(200, "text/html", "Position changed to home");
}

void handle_move_absolute() {
  move_absolute(server.arg(0).toFloat());
  /*
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"deg") {
      move_absolute(server.arg(i).toFloat());
    }
  }
  */
  server.send(200, "text/html", "Position changed to absolute");
}

void handle_move_relative() {
  move_relative(server.arg(0).toFloat());
  /*
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"deg") {
      move_relative(server.arg(i).toFloat());
    }
  }
  */
  server.send(200, "text/html", "Position changed to relative");
}

void handle_move_power() {
  move_power(server.arg(0).toFloat());
  /*
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"power") {
      move_power(server.arg(i).toFloat());
    }
  }
  */
  server.send(200, "text/html", "Position changed to relative power");
}

void handle_move_fwd() {
  move_fwd();
  server.send(200, "text/html", "Moved forward");
}

void handle_move_bwd() {
  move_bwd();
  server.send(200, "text/html", "Moved backward");
}

void handle_move_min() {
  move_min();
  server.send(200, "text/html", "Moved to min");
}

void handle_move_max() {
  move_max();
  server.send(200, "text/html", "Moved to max");
}

// Settings
void handle_set_speed() {
  set_speed(server.arg(0).toInt());
  /*
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"v") {
      set_speed(server.arg(i).toInt());
    }
  }
  */
  server.send(200, "text/html", "Speed set");
}

void handle_set_jog_step() {
  set_jog_step(server.arg(0).toFloat());
  /*
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"step") {
      set_jog_step(server.arg(i).toFloat());
    }
  }
  */
  server.send(200, "text/html", "Jog step set");
}

void handle_set_angle_time() {
  float angle = server.arg(0).toFloat();
  float t = server.arg(1).toFloat();
  /*
  float angle = 100;
  float t = 1;
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"angle") {
      angle = server.arg(i).toFloat();
    }
    else if(server.argName(i) == (const String)"time") {
      t = server.arg(i).toFloat();
    }
  }
  */
  set_angle_time(angle, t);

  server.send(200, "text/html", "Speed changed on sngle and time");
}

void handle_set_min_max() {
  set_min(server.arg(0).toFloat());
  set_max(server.arg(1).toFloat());
  /*
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"min") {
      set_min(server.arg(i).toFloat());
    }
    else if(server.argName(i) == (const String)"max") {
      set_max(server.arg(i).toFloat());
    }
  }
  */
  server.send(200, "text/html", "Min and max set");
}

void handle_set_calibration_ranges() {
  int angle_min = server.arg(0).toInt();
  int angle_max = server.arg(1).toInt();
  /*
  int angle_min;
  int angle_max;
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"min") {
      angle_min = server.arg(i).toInt();
    }
    else if(server.argName(i) == (const String)"max") {
      angle_max = server.arg(i).toInt();
    }
  }
  */
  set_calibration_ranges(angle_min, angle_max);

  server.send(200, "text/html", "calibration ranges set");
}

void handle_calibration_min_max() {
  server.send(200, "text/html", "Calibration call");
  calibration_min_max();
}

void handle_set_trigger_angles() {
  float angle_start = server.arg(0).toFloat();
  float angle_stop = server.arg(1).toFloat();
  float t = server.arg(2).toFloat();
  /*
  float angle_start = 0.;
  float angle_stop = 0.;
  float t = 1.;
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"start") {
      angle_start = server.arg(i).toFloat();
    }
    else if(server.argName(i) == (const String)"stop") {
      angle_stop = server.arg(i).toFloat();
    }
    else if(server.argName(i) == (const String)"time") {
      t = server.arg(i).toFloat();
    }
  }
  */
  set_trigger(angle_start, angle_stop, t);

  move_absolute(angle_start);
}

void handle_set_trigger_power() {
  float power_start = server.arg(0).toFloat();
  float power_stop = server.arg(1).toFloat();
  float t = server.arg(2).toFloat();
  /*
  float power_start = 0.;
  float power_stop = 0.;
  float t = 1.;
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == (const String)"start") {
      power_start = server.arg(i).toFloat();
    }
    else if(server.argName(i) == (const String)"stop") {
      power_stop = server.arg(i).toFloat();
    }
    else if(server.argName(i) == (const String)"time") {
      t = server.arg(i).toFloat();
    }
  }
  */

  float angle_start = power_to_angle(power_start);
  float angle_stop = power_to_angle(power_stop);

  set_trigger(angle_start, angle_stop, t);

  move_absolute(angle_start);
}

// Cicero handler
void handle_getdata() {
  // Server data
  StaticJsonDocument<500> doc;
  doc["server_name"] = "EL14_controller";
  doc["receive_data"] = false;
  JsonArray cmds = doc.createNestedArray("available_commands");
  //for(int i=0; i < sizeof(available_cmds); i++) {
  for(const String &cmd : available_cmds) {
    cmds.add(cmd);
  }
  // json to string
  String serverData;
  serializeJson(doc, serverData);

  /*
  String json = "{\"server_name\":\"EL14_controler\",\"receive_data\":false,\"available_commands\":[";
  for(int i=0; i < sizeof(available_cmds); i++) {
    json += "\"" + available_cmds[i] + "\",";
  }
  json = json.substring(0, -1) + "]}";
  */

  server.send(200, "text/html", serverData);
}

void handle_start() {
  move_trigger_start();
  server.send(200, "text/html", "ok");
}

void handle_addproperty() {
  server.send(200, "text/html", "ok");
}

void handle_end() {
  move_min();
  server.send(200, "text/html", "ok");
}

// Misc
void blink(int n) {
  for(int i=0; i<n; i++) {
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
  }
}

void send_ip() {
  HTTPClient http;

  String url = "http://172.17.32.126:7777/" + WiFi.localIP().toString();

  http.begin(url.c_str());
  http.GET();

  http.end();
}