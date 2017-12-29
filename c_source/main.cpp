#include "GLCD.h"
#include "Board_Buttons.h"
#include "Board_Led.h"
#include <stdio.h>
#include "game_elements.cpp"

void await(unsigned int j){
	int count;
	for(count=0; count<j; count++);
}

void GameInitialization(unsigned int difficulty){
	
	GLCD_Clear(Black);
	int i=2;
	int j=0;
	short brick_color;
	while(i*j<=1210){			//1452 to have 8 rows, 1210 to have 7 rows ecc...
		
		Brick b (i,270-10*j);
		b.draw();
		i+=24;
		if(i==242){
			i=2;
			j++;
		}
	}
	Brick b(218,270-10*j);
	b.draw();
	
	Paddle p(difficulty);
	p.draw();
	
	Ball ball(117,100);
	while(ball.y>=15){
		ball.draw();
		ball.y--;
		delay(1);
	}
}

int main(void){
	
	GLCD_Init();
	LED_Initialize();
	GameInitialization(1);
	while(1){
	}
	return 0;
}
