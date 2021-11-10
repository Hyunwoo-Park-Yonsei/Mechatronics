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

#define PGAIN 40
#define IGAIN 0.3
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
	
	IError += errorPosition * LOOPTIME/1000; // I error는 적분을 해야하므로 왼쪽과 같이 표현
	DError = (tempErrorPosition- errorPosition)/(LOOPTIME); // D error는 미분을 해야하므로 왼쪽과 같이 표현
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
	IError += errorPosition * LOOPTIME/1000; // I error는 적분을 해야하므로 왼쪽과 같이 표현
	DError = (tempErrorPosition- errorPosition)/LOOPTIME;  // D error는 미분을 해야하므로 왼쪽과 같이 표현
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


	printf("first goal: "); // 처음 돌아야하는 바퀴수를 num1 으로 표현
	scanf("%d",&num1);
	printf("second goal: ");  // 두번째로 돌아야하는 바퀴수를 num2 으로 표현
	scanf("%d",&num2);
	pulse = LOW;
	printf("num1 and num2 : %d,%d\n",num1,num2);
	num2 *= -1; // 두번째는 반대방향이므로 -1을 곱해서 표현
	while(1)
	{
		if(pulse == LOW)
		{
			pulse = digitalRead(PULSE); // pulse 업데이트
			if (pulse == HIGH) // pulse가 LOW 에서 HIGH 라면, 즉 처음으로 들어왔다면
			{
				pulseCount +=1; // pulse count를 더한다
			}
		}
		else if(pulse == HIGH)
		{
			pulse = digitalRead(PULSE); //pulse가 HIGH에서 LOW로 다시 업데이트 될 수 있도록, HIGH 상태에서도 계속 업데이트
		}
				
		
		checkTime = millis();
		if (checkTime - checkTimeBefore > LOOPTIME)
		{
			if (pulseCount == 1) //첫 pulse일 때
			{
				referencePosition = num1;
				errorPosition = referencePosition - redGearPosition;
				//printf("Current Motor Position: %f\n",redGearPosition);
				//printf("temp error: %f\n",tempErrorPosition);
				//printf("error: %f\n",errorPosition);
				//printf("I error: %f\n",IError);
				//printf("D error: %f\n",DError);

				if (errorPosition > 0) // error 가 양수라면 양의 방향으로 회전
				{
					softPwmWrite(MOTOR1, errorPosition*PGAIN+IError*IGAIN +DError*DGAIN);
					softPwmWrite(MOTOR2, 0);
				}
				else // error 가 음수라면 음의 방향으로 회전
				{
					softPwmWrite(MOTOR2, -errorPosition*PGAIN-IError*IGAIN-DError*DGAIN);
					softPwmWrite(MOTOR1, 0);
				}
				checkTimeBefore = checkTime;
			}
			else if (pulseCount == 2) // 두번째 pulse 일 때
			{
				referencePosition = num2;
				errorPosition = referencePosition - redGearPosition;

				//printf("Current Motor Position: %f\n",redGearPosition);
				//printf("temp error: %f\n",tempErrorPosition);
				//printf("error: %f\n",errorPosition);
				//printf("I error: %f\n",IError);
				//printf("D error: %f\n",DError);
				if (errorPosition > 0) // error 가 양수라면 음의 방향으로 회전
				{
					softPwmWrite(MOTOR1, +errorPosition*PGAIN+IError*IGAIN +DError*DGAIN);
					softPwmWrite(MOTOR2, 0);
				}
				else // error 가 음수라면 양의 방향으로 회전
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

