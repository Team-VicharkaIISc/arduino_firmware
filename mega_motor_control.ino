//Made by The Software Team of Vicharaka
#include<ros.h>//ROS library
#include<RMCS2304.h>//Motor driver library
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
RMCS2304 rmcs;

ros::NodeHandle nh; //Create NodeHandle object from class
std_msgs::String ack;

int state;//Current state of the rover

ros::Publisher ackno("ack_arduino",&ack);//Start ack_arduino node to acknowledge stuff

byte slave_id=7;
int INP_CONTROL_MODE=513;           
int acceleration=75;  // Range of acceleration is 1-128
int speed=2000;      // Range of acceleration is 0-2048

void actuate(const std_msgs::Int16& command){//Callback function called everytime a message is published to /arduino_receiver
if(command.data==0){//We have received command to stop. Get previous state and stop accordingly
  ack.data="ACK: STOP";//Acknowledge that we have the request to stop
  if(state==1){//Were we going forward? Then Brake_Motor with 0
  rmcs.Brake_Motor(slave_id, 0);
  delay(1000);
}
else if(state==3){//Were we going backward? Then Brake_Motor with 1
    rmcs.Brake_Motor(slave_id, 1);
    delay(1000);
  ackno.publish(&ack); //Send our acknowledgement once we have fulfilled the request
}
state=0;//we have stopped. Update state
return;
}

  
state=command.data; //set state to received command

  
if(state==1){
  rmcs.Brake_Motor(slave_id, 0);
  delay(1000);
}
else if(state==3){
    rmcs.Brake_Motor(slave_id, 0);
    delay(1000);

}

  if(state==1){
    //Move forward
    ack.data="ACK: W";   //Acknowledge that we have the request to move forward
      ackno.publish(&ack);

    rmcs.Speed(slave_id,speed);                   //Set speed within range of 0-2048.
    rmcs.Enable_Digital_Mode(slave_id,0);        //To enable motor in digital speed control mode. 0-fwd,1-reverse direction. 
    
  }
  else if(state==2){
    //Turn left
    ack.data="ACK: A"; //Acknowledge that we have the request to turn left
  }
  else if(state==3){
    //Move backward
    ack.data="ACK: S";//Acknowledge that we have the request to move back
      ackno.publish(&ack);

     rmcs.Speed(slave_id,speed);                   //Set speed within range of 0-2048.
    rmcs.Enable_Digital_Mode(slave_id,1);        //To enable motor in digital speed control mode. 0-fwd,1-reverse direction. 
    
  }
  else if(state==4){
    //Turn right
    ack.data="ACK: D"; //Acknowledge that we have received the request to turn right
  }
  else{
    //Illegal command
    ack.data="NO ACKNO"; //We have been sent an invalid command. Cannot fullfill request.
  }

}


ros::Subscriber<std_msgs::Int16> receiver("arduino_receiver",&actuate);//Start /arduino_receiver node


void setup() {
  // put your setup code here, to run once:
  nh.initNode();
  nh.subscribe(receiver);//Subscrive to /arduino_receiver
  nh.advertise(ackno);//Advertise our publisher

  rmcs.Serial_selection(0);       //Serial port selection:0-Hardware serial,1-Software serial
  rmcs.begin(&Serial1,9600);    //hardware serial port for mega2560:Serial1,Serial2,Serial3 and set baudrate Uncomment this line if hardware serial port
  rmcs.Brake_Motor(slave_id, 0);//Stop motor
}

void loop() {
  // put your main code here, to run repeatedly:
  nh.spinOnce();//Keep listening to commands from PI
  delay(1000);
 

}
