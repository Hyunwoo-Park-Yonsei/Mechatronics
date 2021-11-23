#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <time.h>


#define LED_RED 17
#define LED_GREEN 18
#define LED_YELLOW 23
#define SWITCH1 27
#define SWITCH2 25
#define SWITCH3 12

int main(void)
{
	wiringPiSetupGpio();
	pinMode(LED_RED,OUTPUT);
	pinMode(LED_GREEN,OUTPUT);
	pinMode(LED_YELLOW,OUTPUT);
	int score = 5;
	
	for (int k=0; k < 10; k++)
	{
		printf("%d th try \n",k);
		if (score != 5)
		{
			break;
		}
		for (int t =0; t < 5; t++)
		{
			digitalWrite(LED_RED,HIGH);
			digitalWrite(LED_GREEN,HIGH);
			digitalWrite(LED_YELLOW,HIGH);
			delay(500);
			digitalWrite(LED_RED,LOW);
			digitalWrite(LED_GREEN,LOW);
			digitalWrite(LED_YELLOW,LOW);
			delay(500);
		}
		srand((unsigned int)time(NULL));
		
		int ans_arr[5] = {-1,-1,-1,-1,-1};
		for (int i =0; i < 5; i ++)
		{
			int num = rand()%3;
			ans_arr[i] = num;
			
			if (num == 1)
			{
				digitalWrite(LED_RED,HIGH);
				delay(1000-100*k);
				digitalWrite(LED_RED,LOW);
				delay(1000-100*k);
			}
			else if (num == 0)
			{
				digitalWrite(LED_GREEN,HIGH);
				delay(1000-100*k);
				digitalWrite(LED_GREEN,LOW);
				delay(1000-100*k);
			}
			else if (num == 2)
			{
				digitalWrite(LED_YELLOW,HIGH);
				delay(1000-100*k);
				digitalWrite(LED_YELLOW,LOW);
				delay(1000-100*k);
			}
		}
		printf("%d %d %d %d %d \n", ans_arr[0], ans_arr[1], ans_arr[2], ans_arr[3], ans_arr[4] );
		
		int my_ans[5] = {-1,-1,-1,-1,-1};
		
		
		int switch1 = LOW;
		int switch2 = LOW;
		int switch3 = LOW;
		score = 0;
		for (int j =0; j<5; j++)
		{
			time_t start_time;
			start_time = time(NULL);
			
			while(1)
			{	
				if (switch1 == LOW)
				{
					switch1 = digitalRead(SWITCH1);
					if (switch1 == HIGH && time(NULL) - start_time > 0.5)
					{
						my_ans[j] = 0;
						break;
					}
				}
				else if (switch1 == HIGH)
				{
					switch1 = digitalRead(SWITCH1);
				}
				
				if (switch2 ==LOW)
				{
					switch2 = digitalRead(SWITCH2);
					if (switch2 == HIGH && time(NULL) - start_time > 0.5)
					{
						my_ans[j] = 1;
						break;
					}
				}
				else if (switch2 == HIGH)
				{
					switch2 = digitalRead(SWITCH2);
				}
				
				if (switch3 ==LOW)
				{
					switch3 = digitalRead(SWITCH3);
					if (switch3 == HIGH && time(NULL) - start_time > 0.5)
					{
						my_ans[j] = 2;
						break;
					}
				}
				else if (switch3 == HIGH)
				{
					switch3 = digitalRead(SWITCH3);
				}	
			}
			if (my_ans[j] != ans_arr[j])
			{
				break;
			}
			else
			{
				score +=1;
			}
		}
		printf("%d %d %d %d %d \n", my_ans[0], my_ans[1], my_ans[2], my_ans[3], my_ans[4] );
		if (score != 5)
		{
			break;
		}

	}
	if (score !=5)
	{
		while(1)
		{
			digitalWrite(LED_RED,HIGH);
		}
	}
	else
	{
		while(1)
		{
			digitalWrite(LED_GREEN,HIGH);
		}
	}

	return 0;
}
