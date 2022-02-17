# EL14 controler with nodeMCU

<img src="./graphics/EL14.jpg" alt="EL14" width="30%"/><img src="./graphics/nodeMCU.jpg" alt="nodeMCU" 
width="30%"/>

[toc]

## How to use
### Power up
EL14 is managed with commands sent with http protocole over intranet. On boot nodeMCU will try to connect to lab WiFi and set up a server. Server (at most cases) takes ip `172.17.32.174`. If the ip is different, the last part will be near number (173, 175). It can be checked also on router website in connections map section. This ip address should be given to handler.

**Important**. On power-up, nodeMCU boots and state of some of the pins changes. EL14 sees it as an attempt of serial communication and responds with errors. This changes the state of nodeMCU and it doesn't boot properly. **During boot, the devices should be disconnected!**. Half a second is enough, then nodeMCU goes into initialisation procedures and can be connected back. Also, on power-up, EL14 calibrates resonant frequencies and it can take some time. NodeMCU waits for 5 s, but if it is not enough just don't connect the device back before the EL14 stops.

### Pin description
* D0 - in motion state input
* D1 - jog enable output
* D2 - jog forward output
* D3 - jog backward output
* D6 - trigger
* D7 - serial RX (should be connected to TX of EL14)
* D8 - serial Tx (should be connected to RX of EL14)
* A0 - photodiode analog input with ranges 0 - 3.3 V

### Command mode

#### Command syntax
Available commands are listed in section below. Command syntax follows url syntax from http protocole and looks like this: `http://device_ip/command?key1=value1&key2=value2`. For example if device is hosting a server under ip address 172.17.32.174 and one wants to rotate it to absolute position of 72.3 deg, the url will look like this: `http://172.17.32.174/move_absolute?deg=72.3`. If one wants to set new `angle_min` and `angle_max` values, the url will look like this: `http://172.17.32.174/set_min_max?min=0&max=100`.

**Important.** Parameters of commands which need them are obligatory for stable work of the device.

If one wants to use python for control an exemplary script for setting speed with angle-time command, homing the device and rotating by arbitrary angle is shown below.

```python
import requests

url = 'http://172.17.32.174/'
speed_angle = 100 # deg
speed_time = 0.1 # s
angle = 100 # deg

requests.get(
	url + 'set_angle_time',
	params={
		'angle' : speed_angle,
		'time' : speed_time
	}
)
requests.get(
	url + 'home',
	params={
		'dir' : 1
	}
)
requests.get(
	url + 'move_relative',
	params={
		'deg' : angle
	}
)
```

#### Cicero
**TODO commands in Cicero**

### Trigger mode
Device can be triggered on pin D6 which calls external interrupt handling routine. This function stops current job of the nodeMCU and moves the stage to `angle_min` position (this function can be changed in the software). This mode should be used when all other requests are done and device is ready as it may interfere with the request handling. It is useful when one wants to have precise timing of the rotation.

### Flashing new software
Flashing new software faces the same problems as power up part - serial communication cross. For the upload of new software the devices should be disconnected. Current software in PlatfromIO project can be downloaded from gDrive [here](https://drive.google.com/drive/folders/1zUkqIa4bSzibQwh4zjUx1g9AN25MgS32?usp=sharing) or from github [here](https://github.com/einaudi/EL14-rotational-stage-controller.git).

## Available server paths
### `home`
Rotates the stage to home position. By default it is absolute 0 deg.  
Parameters:

* dir - direction of rotation - 0 for clockwise ant 1 for anticlockwise.

### `move_absolute`
Rotates the stage to absolute position.  
Parameters:  

* deg - new absolute position angle

### `move_relative`
Rotates the stage by specified angle from its current position. Angle can be negative.  
Parameters:  

* deg - angle to rotate by

### `move_fwd`
Rotate forward by angle defined as jog step.

### `move_bwd`
Rotate backward by angle defined as jog step.

### `move_min`
Rotate to absolute position defined as `angle_min`. `angle_min` can be set by command or by calibration and is stored in non-volatile memory (it is remembered after power down).

### `move_max`
Rotate to absolute position defined as `angle_max`. `angle_max` can be set by command or by calibration and is stored in non-volatile memory (it is remembered after power down).

### `set_speed`
Set the speed of the device. Value is encoded in percentage of maximal speed. Measured [deg/s] vs [%] calibration is shown in the plot below.  
Parameters:

* v - speed of the device in percentage of maximal speed

![speed_calibration](./graphics/speed_calibration.png)

### `set_jog_step`
Set the angle by which stage rotates when in jog mode.  
Parameters:

* step - jog step to set

### `set_angle_time`
Sets the velocity such that rotation by input angle will take input time.  
Parameters:

* angle - rotation angle
* time - time the rotation by angle should take

### `set_min_max`
Set `angle_min` and `angle_max` parameters. They are also stored in non-volatile memory and will be remembered after power-down.  
Parameters:

* min - angle to be set as `angle_min`
* max - angle to be set as `angle_max`

### `calibration_min_max`
Turns on the calibration of `angle_min` and `angle_max` procedure. The stage goes to 0 deg absolute position and starts to rotate by 1 deg up to 120 deg (in this range it should cover polarizer/half-waveplate range of intensity change). At each position a measurement of light intensity is taken - photodiode must be connected. New values of `angle_min` and `angle_max` are calculated where the intensity is minimal and maximal respectively. If the angular range of calibration is too broad it needs to be changed as the procedure may detect two minimas/maximas and not acquire the following angles correctly.

### `set_calibration_ranges`
Set the ranges of angles for which the calibration is done. Values are stored in non-volatile memory and will be remembered after power-down.  
Parameters:

* min - start angle of calibration
* max - end angle of calibration


