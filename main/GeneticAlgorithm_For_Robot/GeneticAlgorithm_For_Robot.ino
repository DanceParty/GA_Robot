////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         Constricted by the hardware limitations of the Arduino Uno, we were only able to have 32.256 KB of data.           //
//       That combined with unforseen power consumption issues by the power guzzling Arduino server system, left us           //
//        having to use a condensed version of the GA that we had anticipated. The following code generates random            //
//      movement accross a series of five trials using an iterative process. Once properly implimented, the GA will           //
//       arrive at an effective walking actuation through accelerometer input scoring.                                        //
//                                                                                                                            //
//               DEVELOPERS/ENGINEERS: Brandon Schmalzel, Pierce Simpson, Keevan Dance, & Amanda Belles                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>

//variables for accelerometer
const int xpin = A3;                   // x-axis of the accelerometer
const int ypin = A2;                   // y-axis
const int zpin = A1;                   // z-axis (only on 3-axis models)
int x,y,z;
float timestep = 100;                  // scan rate, msec
float timestepS = timestep/1000;
float zero_G = 329;                    // field determined
float scale = 60;                      // "g" field determined
float time1 = 0;
float acc = 0;
float acc_check;
float accx = 0;
float accy = 0;
float accz = 0;
float vel = 0;
float posi = 0;
float distance = 0;

//create server variables
Servo servoOFL;                   //outer front left
Servo servoIFL;                   //inner front left
Servo servoOFR;                   //outer front right
Servo servoIFR;                   //inner front right
Servo servoOBL;                    
Servo servoIBL;
Servo servoOBR;
Servo servoIBR; 

int groupA[2]; 
int groupB[2];
String order;

int led = 5;

int i = 0; 
int mut;
int n = 0;
int d = 0;
int pos_inner = 90;
int pos_outer = 90;

  
//highest distance, highest inner angle, highest outer angle, 
//highest leg order
int highest[4];


void setup()                       //sets all legs to 90 degrees
{ 
  Serial.begin(9600);
  
  //assign servo motors to bread board pins
  servoOFL.attach(2);
  servoIFL.attach(4); 
  servoOFR.attach(7);
  servoIFR.attach(8); 
  servoOBL.attach(9); 
  servoIBL.attach(10); 
  servoOBR.attach(12); 
  servoIBR.attach(13);

  servoOFL.write(90);
  servoIFL.write(90);
  servoOFR.write(90);
  servoIFR.write(90);
  servoOBL.write(90);
  servoIBL.write(90);
  servoOBR.write(90);
  servoIBR.write(90);
  
  pinMode(led, OUTPUT);
  
  //accelerometer setup
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);
} 

void loop()
{
  //DEBUG STRING
  //Serial.println('Start of Loop...'); 
  
  //DEBUG STRING
  //Serial.println('Start of Loop...'); 
  
  int timestamp = 750;
  
  //fitness function variables for loop storage and max dist values
  int dist[5];
  int inner_angle[5];
  int outer_angle[5];
  int leg_order[5];
  
  //DEBUG STRING
  //Serial.println('Set of Fitness variables...'); 
    
  for(n = 0; n < 5; n++)
  {
    //DEBUG STRING
    Serial.println("Entered walking loop..."); 
  
    //simplify legs
    Servo leg1[2] = {servoOFL, servoIFL};
    Servo leg2[2] = {servoOFR, servoIFR};
    Servo leg3[2] = {servoOBL, servoIBL};
    Servo leg4[2] = {servoOBR, servoIBR};
  
    //group inner vs outer to make addressing simpler 
    Servo inner[4] = {servoIFL, servoIFR, servoIBL, servoIBR};
    int innerAngle[4] = {0, 1, 0, 1};
    int outerAngle[4] = {1, 0, 0, 1};
    Servo outer[4] = {servoOFL, servoOFR, servoOBL, servoOBR};
      
    //DEBUG STRING
    //Serial.println("Made through inner/outer..."); 

    //necessary for random number selection each time
    randomSeed(analogRead(0));
    int rnum = random(1,3);
    
    //used in options 1 and 3
    //creates random array each trial of leg order
    int pos[4] = {1, 2, 3, 4};
    
    for (int q = 0; q < 10; q++)
    {
      for (int x = 0; x < 4; x++)
      {
        //necessary for random number selection each time
        randomSeed(analogRead(0));
        //Serial.println("in assignment leg loop");
        int r = random(0,3);
        int temp = pos[x];
        pos[x] = pos[r];
        pos[r] = temp;
     
        //Serial.println(pos[r]);
      }  
    }
    
    //DEBUG STRING
    Serial.println("random leg order..."); 
    Serial.println(pos[0]); 
    Serial.println(pos[1]); 
    Serial.println(pos[2]);
    Serial.println(pos[3]);
    
    //reset start leg angles to 90 degrees
    pos_inner = 90;
    pos_outer = 90;
    
    //If this has been run before
    if(highest[0])
    {
      //DEBUG STRING
      Serial.println("Entered mutation if statement..."); 
  
      mut = 3;
        
      for(i=0; i > 2; i++)
      {
        int m = i + 3;
        
        int angle_inner = highest[1];
        int angle_outer = highest[2];
        String legs = String(highest[3]);
        legs.toCharArray(0, 4);
        
        //DEBUG STRING
        //Serial.println("Assigned in highest..."); 
        //Serial.println(highest[0]); 
        //Serial.println(highest[1]); 
        //Serial.println(highest[2]);
        //Serial.println(highest[3]);
        
        //mutation of highest on INNER ANGLE
        if(rnum == 1)
        {
          //each loop iteration, select random value to test
          int angle_inner = random(40, 140);
          //gives us the degree difference for the mirrored legs
          int diff = abs(angle_inner - pos_inner);
          
          //inner angle
          if(innerAngle[0] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[0] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          //move leg to new angle in increments
          /*-------- Leg A ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[legs[0]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[legs[0]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/            
            if(pos_outer < angle_outer)
            {
              outer[legs[0]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[legs[0]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
          
          //inner angle
          if(innerAngle[1] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[1] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          /*-------- Leg B ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[legs[1]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[legs[1]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[legs[1]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[legs[1]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
          
          //inner angle
          if(innerAngle[2] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[2] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
            
          /*-------- Leg C ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[legs[2]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[legs[2]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
              
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[legs[2]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[legs[2]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
          
          //inner angle
          if(innerAngle[3] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[3] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          /*-------- Leg D ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[legs[3]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[legs[3]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[legs[3]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[legs[3]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          order = String(legs[0]);
          order += String(legs[1]);
          order += String(legs[2]);
          order += String(legs[3]);
          leg_order[m] = order.toInt();
        }
        
        //mutation of highest on OUTER ANGLE
        else if(rnum == 2)
        {
          //each loop iteration, select random value to test
          int angle_outer = random(40, 140);
          //gives us the degree difference for the mirrored legs
          int diff = abs(angle_inner - pos_inner);
    
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
      
          //inner angle
          if(innerAngle[0] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[0] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          //move leg to new angle in increments
          /*-------- Leg A ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[legs[0]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[legs[0]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[legs[0]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[legs[0]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
          
          //inner angle
          if(innerAngle[1] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[1] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
      
          /*-------- Leg B ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[legs[1]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[legs[1]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[legs[1]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[legs[1]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
      
          //inner angle
          if(innerAngle[2] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[2] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          /*-------- Leg C ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[legs[2]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[legs[2]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
              
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[legs[2]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[legs[2]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
      
          //inner angle
          if(innerAngle[3] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[3] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          /*-------- Leg D ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[legs[3]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[legs[3]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[legs[3]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[legs[3]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          order = String(legs[0]);
          order += String(legs[1]);
          order += String(legs[2]);
          order += String(legs[3]);
          leg_order[m] = order.toInt();
        }
        
        //mutation of highest on LEG ORDER
        else{
          //gives us the degree difference for the mirrored legs
          int diff = abs(angle_inner - pos_inner);
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
      
          //inner angle
          if(innerAngle[0] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[0] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          //move leg to new angle in increments
          /*-------- Leg A ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[pos[0]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[pos[0]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[pos[0]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[pos[0]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
      
          //inner angle
          if(innerAngle[1] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[1] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
          
          /*-------- Leg B ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[pos[1]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[pos[1]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90))); 
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[pos[1]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[pos[1]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
      
          //inner angle
          if(innerAngle[2] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[2] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          /*-------- Leg C ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[pos[2]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[pos[2]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[pos[2]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[pos[2]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          //reset start leg angles to 90 degrees
          pos_inner = 90;
          pos_outer = 90;
      
          //inner angle
          if(innerAngle[3] == 1)
          {
            angle_inner = pos_inner + diff;
          }
          else
          {
            angle_inner = pos_inner - diff; 
          }
          
          //outer angle
          if(outerAngle[3] == 1)
          {
            angle_outer = pos_outer + diff;
          }
          else
          {
            angle_outer = pos_outer - diff; 
          }
            
          /*-------- Leg D ---------*/
          while(pos_inner != angle_inner || pos_outer != angle_outer)
          {
            /*-------- INNER ANGLE --------*/
            if(pos_inner < angle_inner)
            { 
              inner[pos[3]].write(pos_inner);
              pos_inner = pos_inner + 1;
              delay(timestamp/(abs(angle_inner-90)));
            }
            
            else if (pos_inner > angle_inner)
            {
              inner[pos[3]].write(pos_inner);
              pos_inner = pos_inner - 1;
              delay(timestamp/(abs(angle_inner-90)));
              
            }
             
            //accel();
            
            /*-------- OUTER ANGLE --------*/
            if(pos_outer < angle_outer)
            {
              outer[pos[3]].write(pos_outer);
              pos_outer = pos_outer + 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
              
            else if (pos_outer > angle_outer)
            {
              outer[pos[3]].write(pos_outer);
              pos_outer = pos_outer - 1;
              delay(timestamp/(abs(angle_outer-90)));
            }
             
            //accel();
          }
          
          order = String(pos[0]);
          order += String(pos[1]);
          order += String(pos[2]);
          order += String(pos[3]);
          leg_order[m] = order.toInt();
        }
 
        inner_angle[n] = angle_inner;
        outer_angle[n] = angle_outer;
        dist[n] = int(distance);
        distance = 0;
        
        //DEBUG STRING
        //Serial.println('Updated mutation trial arrays...');
        //Serial.println(inner_angle[n]);
        //Serial.println(outer_angle[n]);
        //Serial.println(dist[n]);
      }
    }
    
    else{
      mut = 5;
      
      //DEBUG STRING
      //Serial.println("mut variable");
      //Serial.println(mut);
    }
        
    //each loop iteration, select random values to test
    int angle_inner = random(40, 140);
    int angle_outer = random(40, 140);
    inner_angle[n] = angle_inner;
    outer_angle[n] = angle_outer;
    
    //DEBUG STRING
    Serial.println("Random inner and outer angle assignment...");
    Serial.println(angle_inner);
    Serial.println(angle_outer);
  
    //DEBUG STRING
    //Serial.println('Random option variable for option 2...');
    //Serial.println(opt2);
  
    for(i = 0; i < mut; i++)
    {
      Serial.println("Starting trial...");
      
      //gives us the degree difference for the mirrored legs
      int diff = abs(angle_inner - pos_inner);
          
      //reset start leg angles to 90 degrees
      pos_inner = 90;
      pos_outer = 90;
     
      //inner angle
      if(innerAngle[0]== 1)
      {
        angle_inner = pos_inner + diff;
      }
      else
      {
        angle_inner = pos_inner - diff; 
      }
      
      //outer angle
      if(outerAngle[0] == 1)
      {
        angle_outer = pos_outer + diff;
      }
      else
      {
        angle_outer = pos_outer - diff; 
      }
    
      //move leg to new angle in increments
      /*-------- Leg A ---------*/
      while(pos_inner != angle_inner || pos_outer != angle_outer)
      {
        /*-------- INNER ANGLE --------*/            
        if(pos_inner < angle_inner)
        { 
          //DEBUG STRING
          //Serial.println("Leg A pos_inner variable...");
          //Serial.println(pos_inner);
          
          inner[pos[0]].write(pos_inner);
          pos_inner = pos_inner + 1;
          delay(timestamp/(abs(angle_inner-90)));
        }
        
        else if (pos_inner > angle_inner)
        {
          //DEBUG STRING
          //Serial.println("Leg A pos_inner variable...");
          //Serial.println(pos_inner);
          
          inner[pos[0]].write(pos_inner);
          pos_inner = pos_inner - 1;
          delay(timestamp/(abs(angle_inner-90)));
          
        }
         
        //accel();
        
        /*-------- OUTER ANGLE --------*/            
        if(pos_outer < angle_outer)
        {
          //DEBUG STRING
          //Serial.println("Leg A pos_outer variable...");
          //Serial.println(pos_outer);
          
          outer[pos[0]].write(pos_outer);
          pos_outer = pos_outer + 1;
          delay(timestamp/(abs(angle_outer-90)));
        }
          
        else if (pos_outer > angle_outer)
        {
          //DEBUG STRING
          //Serial.println("Leg A pos_outer variable...");
          //Serial.println(pos_outer);
          
          outer[pos[0]].write(pos_outer);
          pos_outer = pos_outer - 1;
          delay(timestamp/(abs(angle_outer-90)));
        }
         
        //accel();
      }
      
      //reset start leg angles to 90 degrees
      pos_inner = 90;
      pos_outer = 90;
      
      //inner angle
      if(innerAngle[1]== 1)
      {
        angle_inner = pos_inner + diff;
      }
      else
      {
        angle_inner = pos_inner - diff; 
      }
      
      //outer angle
       if(outerAngle[1] == 1)
      {
        angle_outer = pos_outer + diff;
      }
      else
      {
        angle_outer = pos_outer - diff; 
      }
           
      /*-------- Leg B ---------*/
      while(pos_inner != angle_inner || pos_outer != angle_outer)
      {
        /*-------- INNER ANGLE --------*/
        if(pos_inner < angle_inner)
        { 
          //DEBUG STRING
          //Serial.println("Leg B pos_inner variable...");
          //Serial.println(pos_inner);
          
          inner[pos[1]].write(pos_inner);
          pos_inner = pos_inner + 1;
          delay(timestamp/(abs(angle_inner-90)));
        }
        
        else if (pos_inner > angle_inner)
        {
          //DEBUG STRING
          //Serial.println("Leg B pos_inner variable...");
          //Serial.println(pos_inner);
          
          inner[pos[1]].write(pos_inner);
          pos_inner = pos_inner - 1;
          delay(timestamp/(abs(angle_inner-90)));
          
        }
         
        //accel();
        
        /*-------- OUTER ANGLE --------*/
        if(pos_outer < angle_outer)
        {
          //DEBUG STRING
          //Serial.println("Leg B pos_outer variable...");
          //Serial.println(pos_outer);
          
          outer[pos[1]].write(pos_outer);
          pos_outer = pos_outer + 1;
          delay(timestamp/(abs(angle_outer-90)));
        }
          
        else if (pos_outer > angle_outer)
        {
          //DEBUG STRING
          //Serial.println("Leg B pos_outer variable...");
          //Serial.println(pos_outer);
          
          outer[pos[1]].write(pos_outer);
          pos_outer = pos_outer - 1;
          delay(timestamp/(abs(angle_outer-90)));
        }
         
        //accel();
      }
      
      //reset start leg angles to 90 degrees
      pos_inner = 90;
      pos_outer = 90;
      
      //inner angle
      if(innerAngle[2]== 1)
      {
        angle_inner = pos_inner + diff;
      }
      else
      {
        angle_inner = pos_inner - diff; 
      }
      
      //outer angle
      if(outerAngle[2] == 1)
      {
        angle_outer = pos_outer + diff;
      }
      else
      {
        angle_outer = pos_outer - diff; 
      }
        
      /*-------- Leg C ---------*/
      while(pos_inner != angle_inner || pos_outer != angle_outer)
      {       
        if(pos_inner < angle_inner)
        { 
          //DEBUG STRING
          //Serial.println("Leg C pos_inner variable...");
          //Serial.println(pos_inner);
          
          inner[pos[2]].write(pos_inner);
          pos_inner = pos_inner + 1;
          delay(timestamp/(abs(angle_inner-90)));
        }
        
        else if (pos_inner > angle_inner)
        {
          //DEBUG STRING
          //Serial.println("Leg C pos_inner variable...");
          //Serial.println(pos_inner);
          
          inner[pos[2]].write(pos_inner);
          pos_inner = pos_inner - 1;
          delay(timestamp/(abs(angle_inner-90)));
          
        }
         
        //accel();
        
        /*-------- OUTER ANGLE --------*/           
        if(pos_outer < angle_outer)
        {
          //DEBUG STRING
          //Serial.println("Leg C pos_outer variable...");
          //Serial.println(pos_outer);
          
          outer[pos[2]].write(pos_outer);
          pos_outer = pos_outer + 1;
          delay(timestamp/(abs(angle_outer-90)));
        }
          
        else if (pos_outer > angle_outer)
        {
          //DEBUG STRING
          //Serial.println("Leg C pos_outer variable...");
          //Serial.println(pos_outer);
          
          outer[pos[2]].write(pos_outer);
          pos_outer = pos_outer - 1;
          delay(timestamp/(abs(angle_outer-90)));
        }
         
        //accel();
      }
      
      //reset start leg angles to 90 degrees
      pos_inner = 90;
      pos_outer = 90;
      
      //inner angle
      if(innerAngle[3]== 1)
      {
        angle_inner = pos_inner + diff;
      }
      else
      {
        angle_inner = pos_inner - diff; 
      }
      
      //outer angle
      if(outerAngle[3] == 1)
      {
        angle_outer = pos_outer + diff;
      }
      else
      {
        angle_outer = pos_outer - diff; 
      }
      
      /*-------- Leg D ---------*/
      while(pos_inner != angle_inner || pos_outer != angle_outer)
      {
        /*-------- INNER ANGLE --------*/ 
        if(pos_inner < angle_inner)
        { 
          //DEBUG STRING
          //Serial.println("Leg D pos_inner variable...");
          //Serial.println(pos_inner);
          
          inner[pos[3]].write(pos_inner);
          pos_inner = pos_inner + 1;
          delay(timestamp/(abs(angle_inner-90)));
        }
        
        else if (pos_inner > angle_inner)
        {
          //DEBUG STRING
          //Serial.println("Leg D pos_inner variable...");
          //Serial.println(pos_inner);
          
          inner[pos[3]].write(pos_inner);
          pos_inner = pos_inner - 1;
          delay(timestamp/(abs(angle_inner-90)));
          
        }
         
        //accel();
        
        /*-------- OUTER ANGLE --------*/           
        if(pos_outer < angle_outer)
        {
          //DEBUG STRING
          //Serial.println("Leg D pos_outer variable...");
          //Serial.println(pos_outer);
          
          outer[pos[3]].write(pos_outer);
          pos_outer = pos_outer + 1;
          delay(timestamp/(abs(angle_outer-90)));
        }
          
        else if (pos_outer > angle_outer)
        {
          //DEBUG STRING
          //Serial.println("Leg D pos_outer variable...");
          //Serial.println(pos_outer);
          
          outer[pos[3]].write(pos_outer);
          pos_outer = pos_outer - 1;
          delay(timestamp/(abs(angle_outer-90)));
        }
         
        //accel();
      }
      
      order = String(pos[0]);
      order += String(pos[1]);
      order += String(pos[2]);
      order += String(pos[3]);
      leg_order[n] = order.toInt();
      
      //reset leg to 90 degrees in increments
      while(pos_inner != 90 || pos_outer != 90)
      { 
        /*-------- INNER ANGLE --------*/
        if(pos_inner < 90)
        { 
          inner[pos[0]].write(pos_inner);
          inner[pos[1]].write(pos_inner);
          inner[pos[2]].write(pos_inner);
          inner[pos[3]].write(pos_inner);
          pos_inner = pos_inner + 1;
          delay(5);
        }
          
        else if (pos_inner > 90)
        {
          inner[pos[0]].write(pos_inner);
          inner[pos[1]].write(pos_inner);
          inner[pos[2]].write(pos_inner);
          inner[pos[3]].write(pos_inner);
          pos_inner = pos_inner - 1;
          delay(5);
        }
         
        //accel();
          
        /*-------- OUTER ANGLE --------*/
        if(pos_inner < 90)
        { 
          outer[pos[0]].write(pos_outer);
          outer[pos[1]].write(pos_outer);
          outer[pos[2]].write(pos_outer);
          outer[pos[3]].write(pos_outer);
          pos_outer = pos_outer + 1;
          delay(5);
        }
          
        else if(pos_outer > 90)
        {
          outer[pos[0]].write(pos_outer);
          outer[pos[1]].write(pos_outer);
          outer[pos[2]].write(pos_outer);
          outer[pos[3]].write(pos_outer);
          pos_outer = pos_outer - 1;
          delay(5);
        }
         
        //accel();
      }
    }
    
    dist[n] = int(distance);
    distance = 0;
    
    Serial.println("--------------------- Distance: ----------------------");
    Serial.println(dist[n]);
    
    Serial.println("--------------------- Inner Angle: ----------------------");
    Serial.println(inner_angle[n]);
    
    Serial.println("--------------------- Outer Angle: ----------------------");
    Serial.println(outer_angle[n]);
    
    Serial.println("--------------------- Leg Order: ----------------------");
    Serial.println(leg_order[n]);
    
    //reset leg to 90 degrees in increments
    while(pos_inner != 90 || pos_outer != 90)
    { 
      /*-------- INNER ANGLE --------*/
      if(pos_inner < 90)
      { 
        inner[pos[0]].write(pos_inner);
        inner[pos[1]].write(pos_inner);
        inner[pos[2]].write(pos_inner);
        inner[pos[3]].write(pos_inner);
        pos_inner = pos_inner + 1;
        delay(5);
      }
        
      else if (pos_inner > 90)
      {
        inner[pos[0]].write(pos_inner);
        inner[pos[1]].write(pos_inner);
        inner[pos[2]].write(pos_inner);
        inner[pos[3]].write(pos_inner);
        pos_inner = pos_inner - 1;
        delay(5);
      }
        
      /*-------- OUTER ANGLE --------*/
      if(pos_inner < 90)
      { 
        outer[pos[0]].write(pos_outer);
        outer[pos[1]].write(pos_outer);
        outer[pos[2]].write(pos_outer);
        outer[pos[3]].write(pos_outer);
        pos_outer = pos_outer + 1;
        delay(5);
      }
        
      else if(pos_outer > 90)
      {
        outer[pos[0]].write(pos_outer);
        outer[pos[1]].write(pos_outer);
        outer[pos[2]].write(pos_outer);
        outer[pos[3]].write(pos_outer);
        pos_outer = pos_outer - 1;
        delay(5);
      }
    }
    
    //DEBUG STRING
    Serial.println("Lights...");
            
    //LED to show 5 second reset period 
    digitalWrite(led, HIGH);
    delay(5);
    digitalWrite(led,LOW);
    delay(5);
    digitalWrite(led, HIGH);
    delay(5);
    digitalWrite(led,LOW);
    delay(5);
    digitalWrite(led, HIGH);
    delay(5);
    digitalWrite(led,LOW);
    delay(5);
    digitalWrite(led, HIGH);
    delay(5);
    digitalWrite(led,LOW);
    delay(5);
    digitalWrite(led, HIGH);
    delay(5);
    digitalWrite(led,LOW);
    delay(5);
    
    Serial.println("Lights Done :D ...");
  }
  
  int temp = 0; 
  int array;
   
  //set highest
  for(d = 0; d > sizeof(dist); d++)
  {
    if(dist[d] > temp)
    {
      temp = dist[d];
      array = d;
    }
  }
  
  if(temp > highest[0])
  {
    highest[0] = dist[array];
    highest[1] = inner_angle[array];
    highest[2] = outer_angle[array];
    highest[3] = leg_order[array]; 
   
    //Serial.println('Highest array dump...');
    //Serial.println(highest[0]);
    //Serial.println(highest[1]);
    //Serial.println(highest[2]);
    //Serial.println(highest[3]); 
  }
   
  //LED to show 5 second reset period 
  digitalWrite(led, HIGH);
  delay(5000);
  digitalWrite(led,LOW);
  delay(5);
}

void accel()
{
  x = analogRead(xpin);
  delay(1);
  y = analogRead(ypin);
  delay(1);
  z = analogRead(zpin);
  delay(1);
  accx = (((float)x-zero_G)/scale)*9.8;
  accy = (((float)y-zero_G)/scale)*9.8;
  accz = (((float)z-zero_G)/scale)*9.8;
  acc_check = sqrt((accx * accx) + (accy * accy) + (accz * accz)); 
  
  if (acc_check < 11.65) { //ADJUST THIS VALUE TO CHANGE SENSITIVITY. HIGHER VALUE MEANS LESS MOVEMENT GETS ACKNOWLEDGED. LOWER VALUE MEANS MORE VIBRATIONS GET PICKED UP AS MOVEMENT.
    posi = 0;  
  }
  
  else{
    vel = acc * timestepS;
    acc = accx;
    posi = (vel * timestepS) + (.5 * (acc * timestepS * timestepS));
  }
  
  distance = distance + posi;
  //delay(timestep); 
}

