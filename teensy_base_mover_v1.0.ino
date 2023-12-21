//Made by The Software Team of Vicharaka
#include<ros_t.h>//ROS library
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <PulsePosition.h>

ros::NodeHandle nh; //Create NodeHandle object from class
std_msgs::String ack;
//NOTE: mode 0 == Manual; Mode 1== Software
int state;//Current state of the rover
void changeMode(const std_msgs::Int16& mode);
void actuate(const std_msgs::Int16& command);
ros::Publisher ackno("ack_arduino",&ack);//Start ack_arduino node to acknowledge stuff
ros::Subscriber<std_msgs::Int16> receiver("arduino_receiver",&actuate);//Start /arduino_receiver node
ros::Subscriber<std_msgs::Int16> receiverMode("rover_mode",&changeMode);


PulsePositionInput ReceiverInput(RISING);
int currentMode=0;
int prevMode=0;

float ReceiverValue[]={0, 0, 0, 0, 0, 0, 0, 0};
int ChannelNumber=0; 
void setDirn(int,int);
void setAllMotors(int,int);
void turnLeft();
void turnRight();
void stopAllMotors();

int defaultPWM=125;
int turnLeftPWM=100;
int turnRightPWM=100;
int currentPWM=0;
int L1CurrentDIRN=0;
int L2CurrentDIRN=0;
int L3CurrentDIRN=0;
int R1CurrentDIRN=0;
int R2CurrentDIRN=0;
int R3CurrentDIRN=0;
int L1CurrentPWM=0;
int R1CurrentPWM=0;
#define L1_PWM 0
#define R1_PWM 1

#define L1_DIRN 6
#define R1_DIRN 7


#define FORWARD HIGH
#define BACKWARD LOW

void changeMode(const std_msgs::Int16& MODE){
  currentMode=MODE.data;
}

void setDirn(int pin,int dirn){
  digitalWrite(pin,dirn);
}


void setMotor(int pwm, int pwmOutPin){
  analogWrite(pwmOutPin,pwm);
}

void setAllMotors(int dirn,int pwm){
  stopAllMotors();
  delay(100);
  setDirn(L1_DIRN,dirn);
  setDirn(R1_DIRN,dirn);
  L1CurrentDIRN=dirn;
  R1CurrentDIRN=dirn;
  for(int i=0;i<=125;i++){
    analogWrite(L1_PWM,i);
    analogWrite(R1_PWM,i);
  delay(10);
  }
  L1CurrentPWM=125;
  R1CurrentPWM=125;
}

void turnLeft(){
  stopAllMotors();
  delay(100);
  setDirn(L1_DIRN,BACKWARD);
  setDirn(R1_DIRN,FORWARD);
  L1CurrentDIRN=BACKWARD;
  R1CurrentDIRN=FORWARD;
for(int i=0;i<=turnLeftPWM;i++){
  analogWrite(L1_PWM,i);
  analogWrite(R1_PWM,i);
  delay(10);
}
L1CurrentPWM=100;
R1CurrentPWM=100;
}

void turnRight(){
  stopAllMotors();
  delay(100);
  setDirn(L1_DIRN,FORWARD);
  setDirn(R1_DIRN,BACKWARD);
  L1CurrentDIRN=FORWARD;
  R1CurrentDIRN=BACKWARD;
for(int i=0;i<=turnLeftPWM;i++){
  analogWrite(L1_PWM,i);
  analogWrite(R1_PWM,i);
  delay(10);
}
L1CurrentPWM=100;
R1CurrentPWM=100;
}

void stopAllMotors(){
for(int i=L1CurrentPWM;i>=0;i--){
analogWrite(L1_PWM,i);
analogWrite(R1_PWM,i);
delay(10);
}
L1CurrentPWM=0;
R1CurrentPWM=0;
}

void actuate(const std_msgs::Int16& command){//Callback function called everytime a message is published to /arduino_receiver
if(prevMode==1){
state=command.data; //set state to received command
switch(state){
case 0:
   ack.data="ACK: STOP";
   ackno.publish(&ack);
   stopAllMotors();
   break;
case 1:
    //Move forward
    ack.data="ACK: W";   //Acknowledge that we have the request to move forward
    ackno.publish(&ack);
    setAllMotors(FORWARD,defaultPWM);
    break;
case 2:
    //Turn left
    ack.data="ACK: A"; //Acknowledge that we have the request to turn left
    ackno.publish(&ack);
    turnLeft();
    break;
case 3:
    //Move backward
    ack.data="ACK: S";//Acknowledge that we have the request to move back
    ackno.publish(&ack);
    setAllMotors(BACKWARD,defaultPWM);
    break;
case 4:
    //Turn right
    ack.data="ACK: D"; //Acknowledge that we have received the request to turn right
    ackno.publish(&ack);
    turnRight();
    break;
default:
    //Illegal command
    ack.data="NO ACKNO"; //We have been sent an invalid command. Cannot fullfill request.
    ackno.publish(&ack);
    break;
  }
}
}

void read_receiver(void){
  ChannelNumber = ReceiverInput.available();
  if (ChannelNumber > 0) {
      for (int i=1; i<=ChannelNumber;i++){
    ReceiverValue[i-1]=ReceiverInput.read(i);
    }
  }

}

int mapVal(float preVal,float DomMin,float DomMax, bool rev){
  if(preVal<DomMin){
    return 0;
  }
  if(preVal>DomMax){
    return 255;
  }
  int newVal=(int)((preVal-DomMin)*(255/(DomMax-DomMin)));
  if(rev==false){
  return newVal;
  }
  else{
    return (255-newVal);
  }
}


void handleRCControl(){
  float Yaw=  ReceiverValue[3];
  float Pitch= ReceiverValue[1];
  if((Pitch>=1375)&&(Pitch<=1385)){
    if(Yaw<1375){
      //Turn Left
      //Right Side Forward and LeftSide Backward
      int YawPWM=mapVal(Yaw,1010,1375,false);
      L1CurrentDIRN=BACKWARD;
      R1CurrentDIRN=FORWARD;
      setDirn(L1_DIRN,BACKWARD);
      setDirn(R1_DIRN,FORWARD);
      analogWrite(L1_PWM,255-YawPWM);
      analogWrite(R1_PWM,YawPWM);
      L1CurrentPWM=255-YawPWM;
      R1CurrentPWM=YawPWM;
    }
    if(Yaw>1385){
      //Turn Right
      //Right Side Backward and Left Side Forward
      int YawPWM=mapVal(Yaw,1385,1870,false);
      L1CurrentDIRN=FORWARD;
      R1CurrentDIRN=BACKWARD;
      setDirn(L1_DIRN,FORWARD);
      setDirn(R1_DIRN,BACKWARD);
      analogWrite(L1_PWM,YawPWM);
      analogWrite(R1_PWM,255-YawPWM);
      L1CurrentPWM=YawPWM;
      R1CurrentPWM=255-YawPWM;
    }
  }
  else{
    if(Pitch>1385){
      //Move Forward
      int PitchPWM=mapVal(Pitch,1385,1870,false);
      L1CurrentDIRN=FORWARD;
      R1CurrentDIRN=FORWARD;
      setDirn(L1_DIRN,FORWARD);
      setDirn(R1_DIRN,FORWARD);
      analogWrite(L1_PWM,PitchPWM);
      analogWrite(R1_PWM,PitchPWM);
      L1CurrentPWM=PitchPWM;
      R1CurrentPWM=PitchPWM;
    }
    if(Pitch<1375){
      //Move Backward
      int PitchPWM=mapVal(Pitch,1010,1375,false);
      L1CurrentDIRN=BACKWARD;
      R1CurrentDIRN=BACKWARD;
      setDirn(L1_DIRN,BACKWARD);
      setDirn(R1_DIRN,BACKWARD);
      analogWrite(L1_PWM,255-PitchPWM);
      analogWrite(R1_PWM,255-PitchPWM);
      L1CurrentPWM=255-PitchPWM;
      R1CurrentPWM=255-PitchPWM;
    }
      
    }
  }
  
void setup() {
  // put your setup code here, to run once:
  nh.initNode();
  nh.subscribe(receiver);//Subscrive to /arduino_receiver
  nh.advertise(ackno);//Advertise our publisher
  nh.subscribe(receiverMode);

  pinMode(L1_PWM,OUTPUT);
  pinMode(R1_PWM,OUTPUT);
  pinMode(L1_DIRN,OUTPUT);
  pinMode(R1_DIRN,OUTPUT);
  pinMode(14,INPUT);
  stopAllMotors();
  ReceiverInput.begin(14);
    Serial.begin(57600);

}

void loop() {
  // put your main code here, to run repeatedly:
  nh.spinOnce();//Keep listening to commands from PI
  if(currentMode==0){
    if(prevMode==1){
      stopAllMotors();
      prevMode=0;
      Serial.println("Mode: Manual Mode ");
    }
    read_receiver();
    handleRCControl();

  }
  if((currentMode==1)&&(prevMode==0)){
    stopAllMotors();
    prevMode=1;
    Serial.println("Mode: Software Handled");
  }

}
