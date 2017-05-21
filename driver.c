#include "core.h"
#include "driver.h"

#define servocycle 20000

int speed=64;
int PWMA=5;
int PWMB=6;
int servopin=4;
int nowpulse = 0;

int STBY = 8;
int AIN1 = 4;
int AIN2 = 7;
int BIN1 = 10;
int BIN2 = 11;

void Car_Stop();

void servoSetpulse(int servopin,int pulse)
{
int i;
for(i=0;i<50;i++)
{
  digitalWrite(servopin,HIGH);
  delayMicroseconds(pulse);
  digitalWrite(servopin,LOW);
  delayMicroseconds(servocycle - pulse);
}
}
void servoSetup()
{
  nowpulse = 2500;
  servoSetpulse(servopin,nowpulse);
}

void camera_up()
{
/*
	if(nowpulse<2500){
		nowpulse+=50;
		servoSetpulse(servopin,nowpulse);
		delay(15);
	}
*/
	nowpulse +=500;
	if(nowpulse < 2500){
		servoSetpulse(servopin,nowpulse);
	}
}
void camera_down()
{
/*
	if(nowpulse>500){
		nowpulse -= 50;
		servoSetpulse(servopin,nowpulse);
		delay(15);
	}
*/
	nowpulse -=500;
	if(nowpulse > 500){
		servoSetpulse(servopin,nowpulse);
	}
}
void setup()
{
 	init();
	pinMode(servopin,OUTPUT);
	servoSetup();

	pinMode(STBY,OUTPUT);
	pinMode(AIN1,OUTPUT);
	pinMode(AIN2,OUTPUT);
	pinMode(BIN1,OUTPUT);
	pinMode(BIN1,OUTPUT);
	pinMode(PWMA,OUTPUT);
	pinMode(PWMB,OUTPUT);

	digitalWrite(STBY,HIGH);
	Car_Stop();
}
void go()
{
	analogWrite(PWMA,speed);
	analogWrite(PWMB,speed);
	digitalWrite(AIN1,HIGH);
	digitalWrite(AIN2,LOW);
	digitalWrite(BIN1,LOW);
	digitalWrite(BIN2,HIGH);	
}
void back()
{
	analogWrite(PWMA,speed);
	analogWrite(PWMB,speed);
	digitalWrite(AIN1,LOW);
	digitalWrite(AIN2,HIGH);
	digitalWrite(BIN1,HIGH);
	digitalWrite(BIN2,LOW);	
}
void left()
{
	analogWrite(PWMA,speed);
	analogWrite(PWMB,speed);
	digitalWrite(AIN1,HIGH);
	digitalWrite(AIN2,LOW);
	digitalWrite(BIN1,HIGH);
	digitalWrite(BIN2,LOW);	
}
void right()
{
	analogWrite(PWMA,speed);
	analogWrite(PWMB,speed);
	digitalWrite(AIN1,LOW);
	digitalWrite(AIN2,HIGH);
	digitalWrite(BIN1,LOW);
	digitalWrite(BIN2,HIGH);	
}
void Car_Stop()
{
	analogWrite(PWMA,0);
	analogWrite(PWMB,0);
	digitalWrite(AIN1,LOW);
	digitalWrite(AIN2,LOW);
	digitalWrite(BIN1,LOW);
	digitalWrite(BIN2,LOW);
}
/*
void servoSetAngle(int angle)
{
	if(angle >= 0 && angle <= 180)
		servoSetpulse(servopin,angle*11+500);
}

void test()
{
	int i;
	for(i=0;i<=40;i++)
		servoSetAngle(i);
}
void servo(int angle)
{
	if(angle>=0 && angle <=40)
	{
		servoSetAngle(angle);
	}
}

void camera_up()
{
	int i = angle;
	angle-=5;
	if(angle>=40)
		angle=40;
	for(;i>angle;i--)
		servo(i);
}
void camera_down()
{
	int i = angle;
	angle+=5;
	if(angle<=0)
		angle=0;
	for(;i<angle;i++)
		servo(i);
}
*/
