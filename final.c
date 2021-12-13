#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <math.h>
#include <stdbool.h>


#define ENCODERA 20
#define ENCODERB 21
#define ENC2REDGEAR 216

#define LOOPTIME 5 //millisec

#define MOTOR1 23
#define MOTOR2 24
#define PULSE 17

int encA;
int encB;
int pulse; // receiving pulse signal
int pulseCount = 0; //counting pulse signal
int encoderPosition = 0;
float redGearPosition = 0;
int k =0;

float refArr[100000] = {0}; //array that keeps recorded reference positions of motor
float encPosArr[100000] = {0}; // array that keeps following positijon of motor for RMS calculation and velocity filtering(moving pass filter)

int idx = 0; // index for refArr[]
float error =0; // position error
float max_error = 0; // max position error for collision check 

float vel_sum = 0; // sum of velocity for moving pass filter


float encoder_velocity = 0; // velocity of encoder
float temp_encoder_position = 0; // velocity of encoder one time step before
float filterd_vel =0; // filtered velocity
float score = 0; // RMS score

unsigned int startTime; // time variable for fixing an iteration time to 100ms
int max_time = 0; // maximum iteration time
int count = 0; // variable for counting pseudo-collsion. If count is more than certian constant, it is considered collision.

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
	
	//printf("funcENCODERA() A: %d B: %d encPos: %d gearPos: %f\n",encA,encB,encoderPosition,redGearPosition);
}


int main()
{
	wiringPiSetupGpio();
	
	pinMode(ENCODERA, INPUT);
	pinMode(ENCODERB, INPUT);
	
	softPwmCreate(MOTOR1, 0, 100);
	softPwmCreate(MOTOR2, 0, 100);
	
	wiringPiISR(ENCODERA, INT_EDGE_BOTH,funcEncoderA);
	wiringPiISR(ENCODERB, INT_EDGE_BOTH,funcEncoderB);
	pulse = LOW;
	pulseCount = 0;
	printf("wating for pulse \n");
	while(1)
	{

		if (pulse ==LOW) // if pulse is low, which means switch isn't pushed yet.
		{
			pulse = digitalRead(PULSE); // update pulse signal
			if (pulse == HIGH)
			{
				pulseCount ++; // if pulse signal is received, add pulse count.
			}
		}
		else if(pulse ==HIGH) // if pulse is low, which means switch is pushed.
		{
			pulse = digitalRead(PULSE); //pulse variable has to be get back to low.
		}
		if (pulseCount == 1 && idx < 100000) // if switch is pushed for the first time. // idx condition is for keeping this contion from repeating between first pulse and second pulse. 
		{
			printf("first pulse detected \n");
			encoderPosition = 0; // initialize the encoder position.
			redGearPosition = 0;
			startTime = micros(); // to fix the iteration time.
			while(idx < 100000)
			{
				//printf("%f\n",redGearPosition);
				
				
				
				refArr[idx] = redGearPosition; // recording ref position
				//printf("time %d \n",micros() - startTime);
				idx++;
				while (micros() - startTime < 100)
				{
					continue; // This iteration makes iteration to have fixed duration time.
				}
				startTime = micros(); //updated strat time.
				
				 
			}
			printf("record finished \n");
		}
		else if(pulseCount == 2 && k ==0) // if switch is pushed for the second time. k condition is for keeping this contion from repeating after second pulse. 
		{
			//printf("%d \n",k);
		    printf("second pulse detected\n");
			startTime = micros();
			encoderPosition = 0; // initialize encoder position
			redGearPosition = 0;
			for(int k =0; k < 100000; k++)
			{
				startTime = micros();
				error = refArr[k] - redGearPosition; // update error
				if (max_error < error)
				{
					max_error = error; // recording max error
				}
				if (error > 0)
				{
					softPwmWrite(MOTOR1, error*2000); // p control
					softPwmWrite(MOTOR2, 0);
				}
				else if (error <= 0)
				{
				softPwmWrite(MOTOR2, -error* 2000); // p control
					softPwmWrite(MOTOR1, 0);
				}
				
				encPosArr[k] = redGearPosition; // recording following motor's position
				score += (encPosArr[k] - refArr[k])*(encPosArr[k] - refArr[k]); // rms score calculation
				// moving pass filter
				// make an average of 100 time steps velocity.
				if (k <100) // if the iteration just started and it doesn't have enough velocity data, then it makes average with what it has.
				{
					encoder_velocity = (redGearPosition - temp_encoder_position)/0.01; // it is not an accurate velocity but proportional to velocity value. This proportional value makes collision check easier.
					vel_sum += encoder_velocity; // calculation of sum of velocity
					filterd_vel = 1000*vel_sum/k; // filtering velocity
					temp_encoder_position = redGearPosition; //updating temp encoder position
				}
				else
				{
					encoder_velocity = (redGearPosition - temp_encoder_position)/0.01; // if it has 100 velocity data is remove oldest data and add newest data.
					vel_sum += encoder_velocity; // adding newest data
					vel_sum -= (encPosArr[k-99]-encPosArr[k-100])/0.01; // removing old data
					filterd_vel = 1000 *vel_sum/100; // filtering velocity
					temp_encoder_position = redGearPosition; // updating temp encoder position
				}
				/*
				if (k % 500 == 0)
				{
					printf("%f vel %f \n",error*100,filterd_vel);
				}
				*/
				if (fabs(error*100-filterd_vel) >20 && fabs(filterd_vel) <1 )
				{
					count++;
					/* 
						The error and velocity should be proportional since I used P control.
						But because of the moment of inertia of motor it isn't always proportional
						So this condition counts a psedo-collision which includes actual collisions and malfunction due to moment of inertia of motor.
						and if this count is more than 1000, It is considered as actual collsion.
					*/
				}
				else
				{
					count = 0; // update count.
				}
				if (count > 1000)
				{
					printf("malfunction Caution!! \n");
					softPwmWrite(MOTOR1, 0);
					softPwmWrite(MOTOR2, 0);
					break;
					
				}


				if (max_time < micros() -startTime)
				{
					max_time = micros() -startTime;
				}
				while (micros() - startTime < 100) // fixing duration tiem of iteration.
				{
					continue;
				}
				
			}
			softPwmWrite(MOTOR1, 0);
			softPwmWrite(MOTOR2, 0);
			printf("following finished\n");
			printf("max error %f \n",max_error); 
			printf("max time %d \n", max_time);
			printf("score %f",score*ENC2REDGEAR/100000);
			printf("\n");
			k = 1;
			
		}
	}
	return 0;
}
