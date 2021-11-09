#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h> 

#define ENCODERA 20
#define ENCODERB 21
#define ENC2REDGEAR 216

#define LOOPTIME 5 //millisec

#define MOTOR1 23
#define MOTOR2 24
#define PULSE 17

#define PGAIN 10
#define IGAIN 0.0347
#define DGAIN 1000


int encA;
int encB;
int pulse;
int pulseCount = 0;
int encoderPosition = 0;
float redGearPosition = 0;

float referencePosition = 10;
int num1;
int num2;
float errorPosition =0;
float tempErrorPosition = 0;
float IError = 0;
float DError = 0;

unsigned int startTime;
unsigned int checkTime;
unsigned int checkTimeBefore;

void funcEncoderA(){
	encA = digitalRead(ENCODERA);
	encB = digitalRead(ENCODERB);
	
	if (encA == HIGH){
		if (encB == LOW){
			encoderPosition++;
		}
		else{
			encoderPosition--;
		}
	}
	else{
		if (encB == LOW){
			encoderPosition--;
		}
		else{
			encoderPosition++;
		}
	}
	redGearPosition = (float)encoderPosition / ENC2REDGEAR;
	
	errorPosition = referencePosition - redGearPosition;
	IError += errorPosition * LOOPTIME/1000;
	DError = (tempErrorPosition- errorPosition)/(LOOPTIME);
	tempErrorPosition = errorPosition;
	//printf("funcENCODERA() A: %d B: %d encPos: %d gearPos: %f\n",encA,encB,encoderPosition,redGearPosition);
}

void funcEncoderB(){
	encA = digitalRead(ENCODERA);
	encB = digitalRead(ENCODERB);
	
	if (encB == HIGH){
		if (encA == LOW){
			encoderPosition--;
		}
		else{
			encoderPosition++;
		}
	}
	else{
		if (encA == LOW){
			encoderPosition++;
		}
		else{
			encoderPosition--;
		}
	}
	redGearPosition = (float)encoderPosition / ENC2REDGEAR;
	
	errorPosition = referencePosition - redGearPosition;
	IError += errorPosition * LOOPTIME/1000;
	DError = (tempErrorPosition- errorPosition)/LOOPTIME;
	tempErrorPosition = errorPosition;
	//printf("funcENCODERA() A: %d B: %d encPos: %d gearPos: %f\n",encA,encB,encoderPosition,redGearPosition);
}


int main()
{
	wiringPiSetupGpio();
	startTime = millis();
	
	pinMode(ENCODERA, INPUT);
	pinMode(ENCODERB, INPUT);
	
	softPwmCreate(MOTOR1, 0, 100);
	softPwmCreate(MOTOR2, 0, 100);
	
	wiringPiISR(ENCODERA, INT_EDGE_BOTH,funcEncoderA);
	wiringPiISR(ENCODERB, INT_EDGE_BOTH,funcEncoderB);
	
	checkTimeBefore = millis();
	errorPosition = referencePosition - redGearPosition;
	IError = errorPosition;
	DError = errorPosition/(LOOPTIME);
	printf("first goal: ");
	scanf("%d",&num1);
	printf("second goal: ");
	scanf("%d",&num2);
	pulse = LOW;
	printf("num1 and num2 : %d,%d\n",num1,num2);
	while(1)
	{
		if(pulse == LOW)
		{
			pulse = digitalRead(PULSE);
			if (pulse == HIGH)
			{
				pulseCount +=1;
			}
		}
		else if(pulse == HIGH)
		{
			pulse = digitalRead(PULSE);
		}
				
		
		checkTime = millis();
		if (checkTime - checkTimeBefore > LOOPTIME)
		{
			if (pulseCount == 1)
			{
				referencePosition = num1;
				printf("Current Motor Position: %f\n",redGearPosition);
				//printf("temp error: %f\n",tempErrorPosition);
				//printf("error: %f\n",errorPosition);
				printf("I error: %f\n",IError);
				printf("D error: %f\n",DError);
				if (errorPosition > 0)
				{
					softPwmWrite(MOTOR1, errorPosition*PGAIN+IError*IGAIN +DError*DGAIN);
					softPwmWrite(MOTOR2, 0);
				}
				else
				{
					softPwmWrite(MOTOR2, -errorPosition*PGAIN-IError*IGAIN-DError*DGAIN);
					softPwmWrite(MOTOR1, 0);
				}
				checkTimeBefore = checkTime;
			}
			else if (pulseCount == 2)
			{
				referencePosition = num2;
				printf("Current Motor Position: %f\n",redGearPosition);
				//printf("temp error: %f\n",tempErrorPosition);
				//printf("error: %f\n",errorPosition);
				printf("I error: %f\n",IError);
				printf("D error: %f\n",DError);
				if (errorPosition > 0)
				{
					softPwmWrite(MOTOR1, errorPosition*PGAIN+IError*IGAIN +DError*DGAIN);
					softPwmWrite(MOTOR2, 0);
				}
				else
				{
					softPwmWrite(MOTOR2, -errorPosition*PGAIN-IError*IGAIN-DError*DGAIN);
					softPwmWrite(MOTOR1, 0);
				}
				checkTimeBefore = checkTime;
			}
		}
	}
	return 0;
}

