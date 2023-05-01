#include<Servo.h>




Servo servo;
int value = 0;
int flexpin1 = A5;

void setup(){
  servo.attach(7);
  Serial.begin(115200);
  Serial.println("Serial Ready!");
  servo.write(30);
}

String input;
int index =0;

void loop(){
  int flexVal1;

  flexVal1 = analogRead(flexpin1);
  
  //100~200
  //servo.write(flexVal1-60);

  input = Serial.readStringUntil('\n');
  //Serial.print("Serial input :" );
  //Serial.println(input);


  if (input == "2"){
    servo.write(30);
    
  }
  else if(input == "3" ){
    servo.write(40);

  }
  else if(input == "4" ){
    servo.write(60);
 
  }
  else  if(input == "5" ){ 
    servo.write(80);
   
  }
  
  if(flexVal1 < 180) Serial.print(0);
  else Serial.print(1);
  //delay(100);
  
}