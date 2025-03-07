#define IR1 45
#define IR2 43
#define IR3 41
#define IR4 39
#define IR5 37
#define IR6 35
#define IR7 33
#define IR8 31

#define LMotorA 51
#define LMotorB 53
#define LMotorPWM 2

#define RMotorA 49
#define RMotorB 47
#define RMotorPWM 3

#define MAX_SPEED 240

int IR_val[8] = {0,0,0,0,0,0,0,0};
int pre_IR_val[8] = {0,0,0,0,0,0,0,0};
int IR_weights[8] = {-20,-15,-10,-5,5,10,15,20};

int MotorBasespeed = 90;


int LMotorSpeed = 0;
int RMotorSpeed = 0;
int speedAdjust = 0;



float P,I,D;
float error;
float previousError =0;
float Kp = 2.5;
float Kd = 0;
float Ki = 0;

void read_IR();
void PID_control();
void mdrive();
void stop();
void turn_left();
void turn_right();
bool noLineDetected();

bool t = false;


void setup() {
  Serial.begin(9600);
  

  pinMode(LMotorA, OUTPUT);
  pinMode(LMotorB, OUTPUT);
  pinMode(LMotorPWM, OUTPUT);

  pinMode(RMotorA, OUTPUT);
  pinMode(RMotorB, OUTPUT);
  pinMode(RMotorPWM, OUTPUT);

}

void loop() {
  if(t){
    line_follow();
  }
  else{
    t = true;
  }
  

}

bool noLineDetected() {
  return (IR_val[0] == 0 && IR_val[1] == 0 && IR_val[2] == 0 && IR_val[3] == 0 && IR_val[4] == 0 && IR_val[5] == 0 && IR_val[6] == 0 && IR_val[7] == 0);
}

void read_IR(){
  IR_val[0] = !digitalRead(IR1);
  IR_val[1] = !digitalRead(IR2);
  IR_val[2] = !digitalRead(IR3);
  IR_val[3] = !digitalRead(IR4);
  IR_val[4] = !digitalRead(IR5);
  IR_val[5] = !digitalRead(IR6);
  IR_val[6] = !digitalRead(IR7);
  IR_val[7] = !digitalRead(IR8);

   Serial.print(IR_val[0]);
   Serial.print(" ");
   Serial.print(IR_val[1]);
   Serial.print(" ");
   Serial.print(IR_val[2]);
   Serial.print(" ");
   Serial.print(IR_val[3]);
   Serial.print(" ");
   Serial.print(IR_val[4]);
   Serial.print(" ");
   Serial.print(IR_val[5]);
   Serial.print(" ");
   Serial.print(IR_val[6]);
   Serial.print(" ");
   Serial.println(IR_val[7]);
}

void PID_control(){

  error = 0;
  
  for (int i=0; i<8; i++){
    error += IR_weights[i] * IR_val[i];
    }

  P = error;
  I = I + error;
  D = error - previousError;

  previousError = error;

  speedAdjust = Kp * P + Ki * I + Kd * D;

  
}

void mdrive(int ml, int mr){
  if (ml > 0) {
    if (ml > 255) {
      ml = 255;
    }
    digitalWrite(LMotorA, HIGH);
    digitalWrite(LMotorB, LOW);
    analogWrite(LMotorPWM, ml);
  }
  else {
    if (ml < -255) {
      ml = -255;
    }
    digitalWrite(LMotorA, LOW);
    digitalWrite(LMotorB, HIGH);
    analogWrite(LMotorPWM, -1*ml);

  }
  if (mr > 0) {
    if (mr > 255) {
      mr = 255;
    }
    digitalWrite(RMotorA, HIGH);
    digitalWrite(RMotorB, LOW);
    analogWrite(RMotorPWM, mr);
  }
  else {
    if (mr < -255) {
      mr = -255;
    }
    digitalWrite(RMotorA, LOW);
    digitalWrite(RMotorB, HIGH);
    analogWrite(RMotorPWM, -1*mr);
  }
}

void turn_right(){
  mdrive(100,100);
  delay(500);
  read_IR();
  while ( IR_val[3] == 0 || IR_val[2] == 0){
    read_IR();
    mdrive(150, -150);
    delay(10);
  }
}

void turn_left(){
  mdrive(100,100);
  delay(500);
  read_IR();
  while ( IR_val[5] == 0 || IR_val[4] == 0){
    read_IR();
    mdrive(-150, 150);
    delay(10);
  }
}

void line_follow(){
  read_IR();

  if(IR_val[0] == 1 && IR_val[1] == 1 && IR_val[2] == 1 && IR_val[3] == 1 && IR_val[4] == 1 && IR_val[5] == 1 && IR_val[6] == 1 && IR_val[7] == 1){
    mdrive(150,150);
    delay(1000);
    while (!noLineDetected()){
      read_IR();
      mdrive(-150,150);
    } 
    turn_left();
  }  

  else if(IR_val[0] == 1 && IR_val[1] == 1 && IR_val[2] == 1 && IR_val[3] == 1 && IR_val[4] == 1 && IR_val[5] == 1){
    mdrive(150,150);
    delay(1000);
    while(!noLineDetected()){
      read_IR();
      mdrive(-150, 150);
    }
    while ( IR_val[5] == 0 || IR_val[4] == 0){
      read_IR(); // Corrected function call
      mdrive(-150, 150);
      delay(10);
    }
  }

  // no line detection
  else if ( IR_val[0] == 0 && IR_val[1] == 0 && IR_val[2] == 0 && IR_val[3] == 0 && IR_val[4] == 0 && IR_val[5] == 0 && IR_val[6] == 0 && IR_val[7] == 0){
    if (previousError<0){
      mdrive(150,150);
      delay(500);
      turn_left();
      
    }else{
      mdrive(150,150);
      delay(500);
      turn_right();
    }
  }
  else{
    PID_control();
    int ml = 120;
    int mr = 120;

    mdrive(ml+speedAdjust, mr-speedAdjust);
  }

  read_IR();
  PID_control();
  int ml = 200;
  int mr = 200;

  mdrive(ml+speedAdjust, mr-speedAdjust);

}