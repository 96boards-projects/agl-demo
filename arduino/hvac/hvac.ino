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

// data format: l:<left_temp>&r:<right_temp>&f:<fan_speed>&
void loop()
{
  if (Serial.available() > 0) {
    for(int i=1; i<=3; i++) {
      String data = Serial.readStringUntil(':');
      if(data != "") {
        String val = Serial.readStringUntil('&');
	if (data == "l")
	  left_temp(val.toInt());
        else if (data == "r")
          right_temp(val.toInt());
        else if (data == "f")
	  fan_speed(val.toInt());
      }
    }
  }
}
