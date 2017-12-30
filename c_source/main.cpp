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
	uint32_t direction_left, direction_right;
	int i=2;
	int j=0;
	short brick_color;
	while(i*j<=1210){			//1452 to have 8 rows, 1210 to have 7 rows ecc...
		
		Brick b_i(i,270-10*j);
		b_i.draw();
		i+=24;
		if(i==242){
			i=2;
			j++;
		}
	}
	Brick b_i(218,270-10*j);
	b_i.draw();
	
	Paddle p(difficulty);
	p.draw();
	
	Ball ball(117,100);
	while(ball.y>15){
		ball.old_y = ball.y+1;
		ball.old_x = ball.x;
		ball.draw();
		ball.y--;
		delay(1);
	}
	
	while(1){
		direction_left = Button_GetState(0);				//state of LEFT
		direction_right = Button_GetState(1);				//state of RIGHT
		p.move(direction_left,direction_right);
		delay(2);
	}
	
}

int main(void){
	
	GLCD_Init();
	Buttons_Initialize();
	GameInitialization(1);
	while(1){
	}
	return 0;
}
