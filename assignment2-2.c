#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <time.h>
#include <softPwm.h>


#define LED_RED 17
#define LED_GREEN 18
#define LED_YELLOW 23
#define SWITCH1 27
#define SWITCH2 25


int charge(time_t * start_time)
{
	int switch1 = LOW;	
	int switch2 = LOW;
	
	
	for (int t= 0; t < 300; t ++)
	{
		switch1 = digitalRead(SWITCH1);
		switch2 = digitalRead(SWITCH2);
		if (t < 100)
		{
			softPwmWrite(LED_GREEN, t);
			delayMicroseconds(30000);
		}
		else if (t < 200)
		{
			softPwmWrite(LED_GREEN, 100);
			softPwmWrite(LED_RED, t-100);
			delayMicroseconds(30000);
		}
		else if (t < 300)
		{
			softPwmWrite(LED_GREEN, 100);
			softPwmWrite(LED_RED, 100);
			softPwmWrite(LED_YELLOW, t-200);
			delayMicroseconds(30000);
		}
		if (switch1 ==LOW)
		{
			switch1 = digitalRead(SWITCH1);
			if (switch1 == HIGH && time(NULL) -*start_time > 0.5)
			{
				*start_time = time(NULL);
				return t;
			}
		}
		if (switch2 ==LOW )
		{
			switch2 = digitalRead(SWITCH2);
			if (switch2 == HIGH && time(NULL) -*start_time > 0.5)
			{
				*start_time = time(NULL);
				return -2;
			}
		}	
	}
	return -1;
}


int main(void)
{
	wiringPiSetupGpio();
	softPwmCreate(LED_GREEN,0,100);
	softPwmCreate(LED_RED,0,100);
	softPwmCreate(LED_YELLOW,0,100);
	
	srand((unsigned int)time(NULL));
	int total_distance = rand()%500;
	total_distance +=2000;
	
	float current_distance = 0;
	int driver = 1;

	printf("Game Start!\n You can Choose driver \n");
	printf("Long Driver: 0~500m // Short Driver: 0~ 50m\n");
	printf("Target distance: %d \n",total_distance);
	

	
	while(total_distance - current_distance > 10)
	{
		int t = -3;
		time_t start_time;
		start_time = time(NULL);
		while (t < 0)
		{
			if (driver == 1 && t== -2)
			{
				printf("Target distance: %d, Your distance: %f, Long Driver Selected \n",total_distance, current_distance);
			}
			else if (driver == -1 && t== -2)
			{
				printf("Target distance: %d, Your distance: %f, Short Driver Selected \n",total_distance, current_distance);
			}

			t = charge(&start_time);
			softPwmWrite(LED_GREEN, 0);
			softPwmWrite(LED_RED, 0);
			softPwmWrite(LED_YELLOW, 0);
			if (t< 0)
			{
				if (t == -2)
				{
					driver *= -1;
				}

			}
			else
			{
				if (driver >0)
				{
					current_distance += t*500/300;
				}
				else
				{
					current_distance += t*50/300;
				}
				printf("Target distance: %d, Your distance: %f, remain distance: %f \n",total_distance, current_distance, total_distance-current_distance);

			}
		}
		
	}
	printf("Target distance: %d, Your distance: %f\n",total_distance, current_distance);
	if (current_distance - total_distance > 10)
	{
		printf("LOSE!");
	}
	else
	{
		printf("WIN!");
	}
		



	return 0;
}
