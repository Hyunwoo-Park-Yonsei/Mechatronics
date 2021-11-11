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
	
	IError += errorPosition * LOOPTIME/1000; // I error´Â ÀûºÐÀ» ÇØ¾ßÇÏ¹Ç·Î ¿ÞÂÊ°ú °°ÀÌ Ç¥Çö
	DError = (tempErrorPosition- errorPosition)/(LOOPTIME); // D error´Â ¹ÌºÐÀ» ÇØ¾ßÇÏ¹Ç·Î ¿ÞÂÊ°ú °°ÀÌ Ç¥Çö
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
	IError += errorPosition * LOOPTIME/1000; // I error´Â ÀûºÐÀ» ÇØ¾ßÇÏ¹Ç·Î ¿ÞÂÊ°ú °°ÀÌ Ç¥Çö
	DError = (tempErrorPosition- errorPosition)/LOOPTIME;  // D error´Â ¹ÌºÐÀ» ÇØ¾ßÇÏ¹Ç·Î ¿ÞÂÊ°ú °°ÀÌ Ç¥Çö
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
	IError = 0;
	DError = 0;


	printf("first goal: "); // Ã³À½ µ¹¾Æ¾ßÇÏ´Â ¹ÙÄû¼ö¸¦ num1 À¸·Î Ç¥Çö
	scanf("%d",&num1);
	printf("second goal: ");  // µÎ¹øÂ°·Î µ¹¾Æ¾ßÇÏ´Â ¹ÙÄû¼ö¸¦ num2 À¸·Î Ç¥Çö
	scanf("%d",&num2);
	pulse = LOW;
	printf("num1 and num2 : %d,%d\n",num1,num2);
	num2 *= -1; // µÎ¹øÂ°´Â ¹Ý´ë¹æÇâÀÌ¹Ç·Î -1À» °öÇØ¼­ Ç¥Çö
	while(1)
	{
		if(pulse == LOW)
		{
			pulse = digitalRead(PULSE); // pulse ¾÷µ¥ÀÌÆ®
			if (pulse == HIGH) // pulse°¡ LOW ¿¡¼­ HIGH ¶ó¸é, Áï Ã³À½À¸·Î µé¾î¿Ô´Ù¸é
			{
				pulseCount +=1; // pulse count¸¦ ´õÇÑ´Ù
				IError = 0;
			}
		}
		else if(pulse == HIGH)
		{
			pulse = digitalRead(PULSE); //pulse°¡ HIGH¿¡¼­ LOW·Î ´Ù½Ã ¾÷µ¥ÀÌÆ® µÉ ¼ö ÀÖµµ·Ï, HIGH »óÅÂ¿¡¼­µµ °è¼Ó ¾÷µ¥ÀÌÆ®
		}
				
		
		checkTime = millis();
		if (checkTime - checkTimeBefore > LOOPTIME)
		{
			if (pulseCount == 1) //Ã¹ pulseÀÏ ¶§
			{
				referencePosition = num1;
				errorPosition = referencePosition - redGearPosition;
				//printf("Current Motor Position: %f\n",redGearPosition);
				//printf("temp error: %f\n",tempErrorPosition);
				//printf("error: %f\n",errorPosition);
				//printf("I error: %f\n",IError);
				//printf("D error: %f\n",DError);

				if (errorPosition > 0) // error °¡ ¾ç¼ö¶ó¸é ¾çÀÇ ¹æÇâÀ¸·Î È¸Àü
				{
					softPwmWrite(MOTOR1, errorPosition*PGAIN+IError*IGAIN +DError*DGAIN);
					softPwmWrite(MOTOR2, 0);
				}
				else // error °¡ À½¼ö¶ó¸é À½ÀÇ ¹æÇâÀ¸·Î È¸Àü
				{
					softPwmWrite(MOTOR2, -errorPosition*PGAIN-IError*IGAIN-DError*DGAIN);
					softPwmWrite(MOTOR1, 0);
				}
				checkTimeBefore = checkTime;
			}
			else if (pulseCount == 2) // µÎ¹øÂ° pulse ÀÏ ¶§
			{
				referencePosition = num2;
				errorPosition = referencePosition - redGearPosition;

				//printf("Current Motor Position: %f\n",redGearPosition);
				//printf("temp error: %f\n",tempErrorPosition);
				//printf("error: %f\n",errorPosition);
				//printf("I error: %f\n",IError);
				//printf("D error: %f\n",DError);
				if (errorPosition > 0) // error °¡ ¾ç¼ö¶ó¸é À½ÀÇ ¹æÇâÀ¸·Î È¸Àü
				{
					softPwmWrite(MOTOR1, +errorPosition*PGAIN+IError*IGAIN +DError*DGAIN);
					softPwmWrite(MOTOR2, 0);
				}
				else // error °¡ À½¼ö¶ó¸é ¾çÀÇ ¹æÇâÀ¸·Î È¸Àü
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

