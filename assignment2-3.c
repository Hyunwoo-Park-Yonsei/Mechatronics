#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#define LED_RED 17
#define LED_GREEN 18
#define LED_YELLOW 23
#define SWITCH1 27
#define SWITCH2 25

struct winsize size;

void gamedesc(void);

int main(int argc, char **argv)
{
	wiringPiSetupGpio();
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_YELLOW, OUTPUT);
	
	
	unsigned int gametime;
	int leftstep,gameresult;
	
	gametime = 120000;
	gameresult = 1;
	leftstep =100;
	
	int fall_step = 0; // left -1 right 1
	bool fall = false;
	
	int state0 = LOW;
	int state1 = LOW;
	unsigned int check_time;
	check_time = time(NULL);
	unsigned int fall_time;
	
	ioctl(0, TIOCGWINSZ, &size);
	srand((unsigned int)time(NULL));
	int green_light_time = rand()% 9;
	green_light_time +=1;
	
	gamedesc();
	
	unsigned int startime = millis();
	unsigned int endtime = gametime+startime;
	unsigned int green_light_check_time = millis();
	
	while(1)
	{	
		printf("%f %f \n",(float)(millis() - green_light_check_time)/1000, (float)green_light_time);
		if ((millis() - green_light_check_time)/1000 < green_light_time )//when green light is on
		{
			// turn on green light
			digitalWrite(LED_GREEN,HIGH);
			digitalWrite(LED_RED,LOW);
			
			if (fall == false)//if you are not fall down
			{
				digitalWrite(LED_YELLOW,LOW);
				
				if (state0 == LOW)
				{
					state0 = digitalRead(SWITCH1);
					if (state0 == HIGH && millis() - check_time > 0.1*1000)
					{
						int temp_step = fall_step;
						fall_step = -1;
						if (temp_step == fall_step)
						{
							fall = true;
							fall_time = millis();
						}
						else
						{
							leftstep -=1;
						}
						
						check_time = millis();
					}
				}
				else
				{
					state0 = digitalRead(SWITCH1);
				}
				
				if (state1 == LOW)
				{
					state1 = digitalRead(SWITCH2);
					if (state1 == HIGH && millis() - check_time > 0.1*1000)
					{
						int temp_step = fall_step;
						fall_step = 1;
						if (temp_step == fall_step)
						{
							fall = true;
							fall_time = millis();
						}
						else
						{
							leftstep -=1;
						}
						
						check_time = millis();
					}
				}
				else
				{
					state1 = digitalRead(SWITCH2);
				}
				if (state1 == HIGH && state0 == HIGH)
				{
					fall = true;
					fall_time = time(NULL);
				}

			}
			else // if you fall down
			{
				digitalWrite(LED_YELLOW,HIGH);
			}
		}
		else //when red light is on
		{
			// turn on red light
			digitalWrite(LED_GREEN,LOW);
			digitalWrite(LED_RED,HIGH);
			state0 = digitalRead(SWITCH1);
			state1 = digitalRead(SWITCH2);
			
			if (state0 == HIGH || state1 == HIGH)
			{
				gameresult = -1;
				break;
			}
			if ((millis() - green_light_check_time)/1000 >= green_light_time +2)
			{
				green_light_check_time = millis();
				green_light_time = rand()% 9;
				green_light_time +=1;
			}
		}
		
		
		// fall check
		if (fall == true)
		{
			if (millis() - fall_time > 1000)
			{
				fall = false;
			}
			
		}
		
		//status initialize
		printf("\033c");
		
		//status print
		
		for (int i =0; i < size.ws_col; i++) putchar('-');
		printf("\n");
		printf("leftstep : %d steps, lefttime : %f seconds\n", leftstep,(float)(endtime - millis())/1000);
		
		if (fall == true) //if you are fall down show left time until you stand up
		{
			printf("you fell down!! - wait %f seconds\n",(float)(millis() - fall_time)/1000);
		}
		
		if(leftstep ==0) break;
		if (endtime < millis()) 
		{
			gameresult = 0;
		}
		if (gameresult == 0) break;
	}
	printf("\n");
	for (int i =0; i < size.ws_col; i++) putchar('-');
	printf("\n");
	
	digitalWrite(LED_GREEN,LOW);
	digitalWrite(LED_YELLOW,LOW);
	digitalWrite(LED_RED,LOW);
	
	if (gameresult == 1)
	{
		printf("\n");
		printf("\n");
		printf("GAME END - YOU SURVIVED\n");
		printf("\n");
		for (int i =0; i< size.ws_col; i++) putchar('-');
		printf("\n");
		
		for (int i =0; i <7; i++)
		{
			digitalWrite(LED_GREEN,HIGH);
			delay(100);
			digitalWrite(LED_GREEN,LOW);
			digitalWrite(LED_YELLOW,HIGH);
			delay(100);
			digitalWrite(LED_YELLOW,LOW);
			digitalWrite(LED_RED,HIGH);
			delay(100);
			digitalWrite(LED_RED,LOW);
		}
	}
	else //game over
	{
		printf("\n");
		printf("\n");
		printf("GAME OVER - YOU'RE DEAD \n");
		printf("\n");
		for (int i =0; i <size.ws_col; i++) putchar('-');
		printf("\n");
		
		for (int i =0; i < 5 ; i++)
		{
			digitalWrite(LED_GREEN,HIGH);
			digitalWrite(LED_YELLOW,HIGH);
			digitalWrite(LED_RED,HIGH);
			delay(500);
			digitalWrite(LED_GREEN,LOW);
			digitalWrite(LED_YELLOW,LOW);
			digitalWrite(LED_RED,LOW);
			delay(500);
		}
	}
	
	digitalWrite(LED_GREEN,LOW);
	digitalWrite(LED_YELLOW,LOW);
	digitalWrite(LED_RED,LOW);
	
	return 0;
}

void gamedesc(void)
{
	for (int i =0; i< size.ws_col; i++) putchar('-');
	printf("Red light & Green light GAME\n");
	delay(1000);
	printf("Rule 1 : When Green light is on, you will move towards the finish line.\n");
	delay(1000);
	printf("\n");
	printf("Rule 2 : When Red light is on, you must immediately stop. If not game is ovver.\n");
	delay(1000);
	for (int i =0; i< size.ws_col; i++) putchar('-');
	printf("Game is start in 3 seconds\n");
	delay(1000);
	printf("3\n");
	delay(1000);
	printf("2\n");
	delay(1000);
	printf("1\n");
	delay(1000);
	printf("GAME START\n");
	for (int i =0; i< size.ws_col; i++) putchar('-');
}
	
				
				
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
