// Title: Arduino sketch for AGL HVAC demo
// Author: Manivannan Sadhasivam
// Copyright (c) 2017 Linaro Ltd.
// All rights reserved.

int l_led = 9;
int r_led = 10;
int fan = 8;

void left_temp(int val)
{
  analogWrite(l_led, val * 10);
}

void right_temp(int val)
{
  analogWrite(r_led, val * 10);
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
      String data = Serial.readStringUntil('&');
      if (data != "") {
        data.toCharArray(buffer, 20);
        ret = sscanf(buffer, "%d:%d:%d", l_val, r_val, f_val);
        
        // write HVAC values
        left_temp(l_val);
        right_temp(r_val);
        fan_speed(f_val);
      }
  }
}
