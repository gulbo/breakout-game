#include <stdint.h>
#include "GLCD.h"

#define BRICK_LENGTH 20
#define BRICK_WIDTH 8
#define BRICK_COLOUR_1 Purple
#define BRICK_COLOUR_2 Blue
#define BRICK_COLOUR_3 Cyan
#define BRICK_COLOUR_4 DarkGreen
#define BRICK_COLOUR_5 Green
#define BRICK_COLOUR_6 Yellow
#define BRICK_COLOUR_7 Red

#define PADDLE_Y 5
#define PADDLE_WIDTH 10
#define PADDLE_COLOUR White

#define BALL_COLOUR Magenta
#define BALL_DIAMETER 6

#define BACKGROUND_COLOUR Black



struct Paddle{
	uint16_t x;
	uint16_t length;
	
	//constructor
	Paddle(uint8_t diff){
		switch(diff){
			case 1: //medium
				length = 40;
				x = 100;
				break;
			case 2: //hard
				length = 20;
				x = 110;
				break;
			default: //easy level
				length = 60;
				x = 90;
				break;
		}
	}
	
	void draw(){
		GLCD_DrawRect(PADDLE_Y,old_x,PADDLE_WIDTH,length,BACKGROUND_COLOUR);
		GLCD_DrawRect(PADDLE_Y,x,PADDLE_WIDTH,length,PADDLE_COLOUR); //TODO INVERT X AND Y
	}
	
	void move(bool direction_left, bool direction_right){
		if(direction_left==1&&direction_right==0){							//means right click
			if(x==0);																							//do nothing instead of moving the paddle
			else{		
				old_x = x+PADDLE_WIDTH;
				x-=1;
				draw();
			}
		}
		else if(direction_left==0&&direction_right==1){				//means left click
			if(x+length==SCREEN_HEIGHT);												//do nothing instead of moving the paddle
			else{
				old_x = x;
				x+=1;
				draw();
			}
		}
		else;
	}
	
	private:
	uint16_t old_x;
};
	

struct Ball{
	int16_t x;
	int16_t y;
	int16_t old_x;
	int16_t old_y;
	int32_t speed_x;
	int32_t speed_y;
	
	//constructor
	Ball(int16_t x, int16_t y){
		this->x = x;
		this->y = y;
		speed_x = 1;
		speed_y = 1;
	}
	
	void draw(){
		
		GLCD_DrawRect(old_y,old_x,BALL_DIAMETER,BALL_DIAMETER,BACKGROUND_COLOUR);
		GLCD_DrawRect(y,x,BALL_DIAMETER,BALL_DIAMETER,BALL_COLOUR);
	}
	
	//uses speed parameters to move the ball inside the screen
	//bounces when hits a border
	void move(){
		old_x = x;
		old_y = y;
		x += speed_x;
		y += speed_y;
		
		
		// by now we assume that we have only 45 deg angles
		//check x axis
		if (x+BALL_DIAMETER==SCREEN_HEIGHT){
			x = SCREEN_HEIGHT - BALL_DIAMETER;
			speed_x = -speed_x;
		}
		else if (x < 0){
			x = 0;
			speed_x = -speed_x;
		}
		
		//check y axis
		if (y+BALL_DIAMETER==SCREEN_WIDTH){
			y = SCREEN_WIDTH - BALL_DIAMETER;
			speed_y = -speed_y;
		}
		else if (y < 0){
			y = 0;
			speed_y = -speed_y;
		}
	}
	
};

struct Brick{
	uint16_t x;
	uint16_t y;
	uint32_t colour;
	bool hit;
	bool drawn;
	
	//constructor
	Brick(uint16_t x, int16_t y){
		this->x = x;
		this->y = y;
		switch(y){
			case 270:
				this->colour = BRICK_COLOUR_1;
				break;
			case 260:
				this->colour = BRICK_COLOUR_2;
				break;
			case 250:
				this->colour = BRICK_COLOUR_3;
				break;
			case 240:
				this->colour = BRICK_COLOUR_4;
				break;
			case 230:
				this->colour = BRICK_COLOUR_5;
				break;
			case 220:
				this->colour = BRICK_COLOUR_6;
				break;
			case 210:
				this->colour = BRICK_COLOUR_7;
				break;		
		}
		hit = false;
		drawn = false;
	}
	
	//check if it is hit, in case change the ball direction
	bool check_collision(Ball ball){
		int16_t ball_top = ball.y + BALL_DIAMETER;
		int16_t ball_bottom = ball.y;													//without ball.y-BALL_RADIUS since the reference is already the left-bottom angle
		int16_t ball_right = ball.x + BALL_DIAMETER;
		int16_t ball_left = ball.x;														//as above

		if (is_inside(x,ball_top)){
			// the ball is coming from the TOP
			hit = true;
			drawn = false;
			ball.speed_y = -ball.speed_y;
		}
		else if (is_inside(x,ball_bottom)){
			// the ball is coming from the BOTTOM
			hit = true;
			drawn = false;
			ball.speed_y = -ball.speed_y;
		}
		else if (is_inside(ball_left,y)){
			// the ball is coming from LEFT
			hit = true;
			drawn = false;
			ball.speed_x = -ball.speed_x;
		}
		else if (is_inside(ball_right,y)){
			// the ball is coming from the RIGHT
			hit = true;
			drawn = false;
			ball.speed_x = -ball.speed_x;
		}
		return hit;
	}
	
	void draw(){
		if (!drawn){
			if (hit)
				GLCD_DrawRect(y,x,BRICK_WIDTH,BRICK_LENGTH,BACKGROUND_COLOUR);
			else
				GLCD_DrawRect(y,x,BRICK_WIDTH,BRICK_LENGTH,colour);
			drawn = true;
		}
	}
	
	private:
	bool is_inside(int16_t tx, int16_t ty){
		if (tx < x || tx > x+BRICK_LENGTH)
			return false;
		if (ty < y || ty > y+BRICK_WIDTH)
			return false;
		return true;
	}
};
