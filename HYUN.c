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
	
	IError += errorPosition * LOOPTIME/1000; // I error�� ������ �ؾ��ϹǷ� ���ʰ� ���� ǥ��
	DError = (tempErrorPosition- errorPosition)/(LOOPTIME); // D error�� �̺��� �ؾ��ϹǷ� ���ʰ� ���� ǥ��
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
	IError += errorPosition * LOOPTIME/1000; // I error�� ������ �ؾ��ϹǷ� ���ʰ� ���� ǥ��
	DError = (tempErrorPosition- errorPosition)/LOOPTIME;  // D error�� �̺��� �ؾ��ϹǷ� ���ʰ� ���� ǥ��
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


	printf("first goal: "); // ó�� ���ƾ��ϴ� �������� num1 ���� ǥ��
	scanf("%d",&num1);
	printf("second goal: ");  // �ι�°�� ���ƾ��ϴ� �������� num2 ���� ǥ��
	scanf("%d",&num2);
	pulse = LOW;
	printf("num1 and num2 : %d,%d\n",num1,num2);
	num2 *= -1; // �ι�°�� �ݴ�����̹Ƿ� -1�� ���ؼ� ǥ��
	while(1)
	{
		if(pulse == LOW)
		{
			pulse = digitalRead(PULSE); // pulse ������Ʈ
			if (pulse == HIGH) // pulse�� LOW ���� HIGH ���, �� ó������ ���Դٸ�
			{
				pulseCount +=1; // pulse count�� ���Ѵ�
			}
		}
		else if(pulse == HIGH)
		{
			pulse = digitalRead(PULSE); //pulse�� HIGH���� LOW�� �ٽ� ������Ʈ �� �� �ֵ���, HIGH ���¿����� ��� ������Ʈ
		}
				
		
		checkTime = millis();
		if (checkTime - checkTimeBefore > LOOPTIME)
		{
			if (pulseCount == 1) //ù pulse�� ��
			{
				referencePosition = num1;
				errorPosition = referencePosition - redGearPosition;
				//printf("Current Motor Position: %f\n",redGearPosition);
				//printf("temp error: %f\n",tempErrorPosition);
				//printf("error: %f\n",errorPosition);
				//printf("I error: %f\n",IError);
				//printf("D error: %f\n",DError);

				if (errorPosition > 0) // error �� ������ ���� �������� ȸ��
				{
					softPwmWrite(MOTOR1, errorPosition*PGAIN+IError*IGAIN +DError*DGAIN);
					softPwmWrite(MOTOR2, 0);
				}
				else // error �� ������� ���� �������� ȸ��
				{
					softPwmWrite(MOTOR2, -errorPosition*PGAIN-IError*IGAIN-DError*DGAIN);
					softPwmWrite(MOTOR1, 0);
				}
				checkTimeBefore = checkTime;
			}
			else if (pulseCount == 2) // �ι�° pulse �� ��
			{
				referencePosition = num2;
				errorPosition = referencePosition - redGearPosition;

				//printf("Current Motor Position: %f\n",redGearPosition);
				//printf("temp error: %f\n",tempErrorPosition);
				//printf("error: %f\n",errorPosition);
				//printf("I error: %f\n",IError);
				//printf("D error: %f\n",DError);
				if (errorPosition > 0) // error �� ������ ���� �������� ȸ��
				{
					softPwmWrite(MOTOR1, +errorPosition*PGAIN+IError*IGAIN +DError*DGAIN);
					softPwmWrite(MOTOR2, 0);
				}
				else // error �� ������� ���� �������� ȸ��
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

