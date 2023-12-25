## This Repo Combines all the Arduino and Teensy Codes into one Repository
**teensy_base_mover.ino** : This file contains Codes that will run on the teensy responsible for moving the base of the rover. This Code will Control all of the 6 Motors responsible for the movement Rover.
The Code has been written for integration with the ROS Main System(that will be rinning on the NVIDIA Jetson). Communication is done via Serial Comms, i.e, by connecting the teensy to the JETSON.

Here is a Pictorial representation of the ROS Topics being broadcasted and subscribed to by this Code:

![Screenshot from 2023-12-25 11-47-17](https://github.com/Team-VicharkaIISc/arduino_firmware/assets/40400917/7b450a7e-e37d-43da-929a-b6d99138df9e)

**incoming arrows denote subscribed topics**
**outgoing arrows denote published topics**

**Description of the Topics**

_**ack_arduino**_: Acknoledgement from the Teensy that your Command is Received and the Arduino is trying to act upon it.
_DataType: std_msgs::String_

_**arduino_receiver**_: Receives Command. Currently its still in development.
Commands: \
0 -> STOP \
1 -> FORWARD       
2 -> TURN LEFT \
3 -> BACKWARD \
4 -> TURN RIGHT \
OTHERWISE -> INVALID STATE. ALL MOTORS STOPPED. \
_DataType:std_msgs::Int16_

_**rover_mode**_: You need to set this to **1** to enable Control via Software. To enable Manual Control, set this to **0**. Default state is **0**.
_DataType: std_msgs::Int16_
