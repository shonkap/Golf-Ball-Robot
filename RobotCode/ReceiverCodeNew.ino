#include <EnableInterrupt.h>
//#include <Servo.h> 

#define SERIAL_PORT_SPEED 57600
#define RC_NUM_CHANNELS  5

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3
#define RC_CH5  4

#define RC_CH1_INPUT  A0
#define RC_CH2_INPUT  A1
#define RC_CH3_INPUT  A2
#define RC_CH4_INPUT  A3
#define RC_CH5_INPUT  A4

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

//int failsafe = 0;

//funtions
void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch1() { calc_input(RC_CH1, RC_CH1_INPUT); }
void calc_ch2() { calc_input(RC_CH2, RC_CH2_INPUT); }
void calc_ch3() { calc_input(RC_CH3, RC_CH3_INPUT); }
void calc_ch4() { calc_input(RC_CH4, RC_CH4_INPUT); }
void calc_ch5() { calc_input(RC_CH5, RC_CH5_INPUT); }

void setup() {
  Serial.begin(SERIAL_PORT_SPEED);

  //homemade motorcontroller
  pinMode(10, OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6,OUTPUT);

  pinMode(11, OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3,OUTPUT);

  pinMode(4,INPUT);

  
  //esc1.attach(escpin1,1000,2000);
  //esc2.attach(escpin2,1000,2000);
  //esc1.write(0);
  //esc2.write(0);

  pinMode(RC_CH1_INPUT, INPUT);
  pinMode(RC_CH2_INPUT, INPUT);
  pinMode(RC_CH3_INPUT, INPUT);
  pinMode(RC_CH4_INPUT, INPUT);
  pinMode(RC_CH5_INPUT, INPUT);

  enableInterrupt(RC_CH1_INPUT, calc_ch1, CHANGE);
  enableInterrupt(RC_CH2_INPUT, calc_ch2, CHANGE);
  enableInterrupt(RC_CH3_INPUT, calc_ch3, CHANGE);
  enableInterrupt(RC_CH4_INPUT, calc_ch4, CHANGE);
  enableInterrupt(RC_CH5_INPUT, calc_ch5, CHANGE);
}

void loop() {
  rc_read_values();

  dump();
  drives();
  conveyor();
  flapper();
  
  //can re-add this **********
  //interrupts();
  
  //Serial.print("CH1:"); Serial.print(rc_values[RC_CH1]); Serial.print("\t");
  //Serial.print("CH2:"); Serial.print(rc_values[RC_CH2]); Serial.print("\t");
  //Serial.print("CH3:"); Serial.print(rc_values[RC_CH3]); Serial.print("\t");
  //Serial.print("CH4:"); Serial.println(rc_values[RC_CH4]);

  //delay(200);
  //failsafe++;
  //if(failsafe > 4){
    //Serial.print("error\t");
  //}
}

//3,5,6,9,10,11

void dump() //channel 7
{
  int limitswitch = digitalRead(4);
  if((limitswitch == 0) && (rc_values[RC_CH1] > 1800)){   //down
     analogWrite(8, LOW);
     analogWrite(7, 255);
  }
  else if(rc_values[RC_CH1] < 1200){  //up
    analogWrite(7, LOW);
    analogWrite(8, 255);
  }
  else{
    analogWrite(8, LOW);
    analogWrite(7, LOW);
  }
}

void conveyor() //channel 8
{
  Serial.print("CH4:"); Serial.println(rc_values[RC_CH2]);
  if(rc_values[RC_CH2] > 1800){   //down
     analogWrite(3, 255);
     analogWrite(2, LOW);
  }
  else if(rc_values[RC_CH2] < 1200){  //up
    analogWrite(3, LOW);
    analogWrite(2, 255);
  }
  else{
    analogWrite(3, LOW);
    analogWrite(2, LOW);
  }
}

void flapper()  //channel 10
{
  if(rc_values[RC_CH5] > 1600){  //up
    analogWrite(11, 255);
    analogWrite(12, 0);
  }
  else{
    analogWrite(11, 0);
    analogWrite(12, 0);
  } 
}



//motors on thing
void forward1(int one)
{
      //sideone
      analogWrite(9, 0);
      analogWrite(10, one);
}
void forward2(int two)
{
      //sidetwo
      analogWrite(6, 0);
      analogWrite(5, two);
}
void reverse1(int one)
{
      //sideone
      analogWrite(10, 0);
      analogWrite(9, one);
}
void reverse2(int two)
{
      //sidetwo
      analogWrite(5, 0);
      analogWrite(6, two);
}
//drive setup
void drives(){   
  //tank drive shit
    int xinput = rc_values[RC_CH4];
    int yinput = rc_values[RC_CH3];
    
    if(rc_values[RC_CH3]<900){
      yinput = 1500;
    }
    if(rc_values[RC_CH4]<900){
      xinput = 1500;
    }
    
    if(rc_values[RC_CH4]<1525 && rc_values[RC_CH4]>1475){
      xinput = 1500;
    }
    if(rc_values[RC_CH3]<1525 && rc_values[RC_CH3]>1475){
      yinput = 1500;
    }
    
    if(xinput<1100){
      xinput = 1100;
    }
    if(xinput>1900){
      xinput = 1900;
    }
    
    if(yinput<1100){
      yinput = 1100;
    }
    if(yinput>1900){
      yinput = 1900;
    }
    
    float xvalue = xinput-1500; 
    float yvalue = yinput-1500;
    float throttle = sqrt(abs(xvalue*xvalue) + abs(yvalue*yvalue));
    float percent = abs(xvalue/500);
    
  //end of tank drive shit
    //Serial.print("throttle: ");Serial.print(throttle);Serial.print(" \t");
    //Serial.print("xvalue: ");Serial.print(rc_values[RC_CH4]);Serial.print(" \t");
    //Serial.print("yvalue: ");Serial.print(yvalue);Serial.print(" \t");

  //you can re-add this ******
  //noInterrupts();
  
  if(throttle < 10)
  {
    //Serial.print("zeroded: \t");
    
    forward1(0);
    forward2(0);
    return;
  }
  else
  {
    float newthrottle = (throttle/400)*255;
    if(newthrottle> 255){
      newthrottle = 255;
    }
    //Serial.print("none:"); Serial.print(newthrottle); Serial.print("\t");
    
    if(percent < .06)
    {
      //Serial.print("none:"); Serial.print(newvalue); Serial.print("\t");
      if(rc_values[RC_CH3]>1500){
        reverse1(newthrottle);
        reverse2(newthrottle);
      }
      else{
        forward1(newthrottle);
        forward2(newthrottle);
      }
      
      return;
    }
    //side to side only
    else if(rc_values[RC_CH3]<1550 && rc_values[RC_CH3]>1450)
    {
      if(rc_values[RC_CH4]<1500)
      {
        //checkreverse(1500-throttle,1);
        reverse1(255);
        forward2(255);
        //checkreverse(1500+throttle,2);
        return;
      }
      else
      {
        //checkreverse(1500+throttle,1);
        reverse2(255);
        forward1(255);
        //checkreverse(1500-throttle,2);
        return;
      }
    }

    
    //left
    else if(rc_values[RC_CH4]<1450)
    {
      percent = 1-percent;
      
      if(rc_values[RC_CH3]>1500){
        reverse1(newthrottle*percent);
        reverse2(newthrottle);
      }
      else{
        forward1(newthrottle*percent);
        forward2(newthrottle);
      }
      
      return;
    }
    
    //right
    else if(rc_values[RC_CH4]>1550)
    {
      percent = 1-percent;
      
      if(rc_values[RC_CH3]>1500){
        reverse1(newthrottle);
        reverse2(newthrottle*percent);
      }
      else{
        forward1(newthrottle);
        forward2(newthrottle*percent);
      }
      
      return;
    }
    //done
  }
}



