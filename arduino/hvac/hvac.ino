// Title: Arduino sketch for AGL HVAC demo
// Author: Manivannan Sadhasivam
// Copyright (c) 2017 Linaro Ltd.
// All rights reserved.

//connections
int l_led = 9;
int r_led = 10;
int fan = 8;

int l_val, r_val, f_val;
char buffer[20];

char lookup[16][2] = 
{
{15,20}, {16,40}, {17,60}, {18,80}, {19,100},
{20,120}, {21,140}, {22,160}, {23,180}, {24,200},
{25,210}, {26,220}, {27,230}, {28,240}, {29,250},
{30,255}
};

void left_temp(int val)
{
  int i;
  
  for (i = 0; i < 16; i++) {
    if (val == lookup[i][0])     
      analogWrite(l_led, lookup[i][1]);
  }
} 

void right_temp(int val)
{
  int i;
  
  for (i = 0; i < 16; i++) {
    if (val == lookup[i][0])     
      analogWrite(r_led, lookup[i][1]);
  }
}

void fan_speed(int val)
{
  analogWrite(fan, val);
}

void setup()
{
  pinMode(l_led, OUTPUT);
  pinMode(r_led, OUTPUT);
  pinMode(fan, OUTPUT);
  Serial.begin(9600);
}

// data format: left_temp:right_temp:fan_speed&
void loop()
{
  while(Serial.available() > 0) {
      memset(buffer, 0, sizeof(buffer));
      String data = Serial.readStringUntil('&');
      if (data != "") {
        data.toCharArray(buffer, 20);
        sscanf(buffer, "%d:%d:%d", &l_val, &r_val, &f_val);
        // write HVAC values
        left_temp(l_val);
        right_temp(r_val);
        fan_speed(f_val);
      }
   }
}
