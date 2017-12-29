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
#define BALL_RADIUS 6

#define BACKGROUND_COLOUR Black

struct Brick{
	uint16_t x;
	uint16_t y;
	uint32_t colour;
	bool hit;
	//bool drawn; MAYBE
	
	//constructor
	Brick(uint16_t x, int16_t y){
		this->x = x;
		this->y = y;
		//colour =  TODO calculate the right color using y
		hit = false;
	}
	
	//has been hit
	void been_hit(){
		hit = true;
		colour = BACKGROUND_COLOUR;
	}
	
	void draw(){
		GLCD_DrawRect(y,x,BRICK_WIDTH,BRICK_LENGTH,colour);
	}
	
};

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
	
	void move(){ 	//TODO DECIDE HOW TO IMPLEMENT (IF COSTANT MOVEMENT OR WITH INPUT PARAMETER)
		old_x = x;
		// x = ...
	}
	
	private:
	uint16_t old_x;
};
	

struct Ball{
	uint16_t x;
	uint16_t y;
	int32_t speed_x;
	int32_t speed_y;
	
	//constructor
	Ball(){
		//TODO
	}
	
	void draw(){
		GLCD_DrawRect(old_y,old_x,BALL_RADIUS,BALL_RADIUS,BACKGROUND_COLOUR);
		GLCD_DrawRect(y,x,BALL_RADIUS,BALL_RADIUS,BALL_COLOUR);
	}
	
	//uses speed parameters to move the ball inside the screen
	//bounces when hits a border
	void move(){
		old_x = x;
		old_y = y;
		x = x + speed_x;
		y = y + speed_y;
		
		// by now we assume that we have only 45 deg angles
		//check x axis
		if (x > SCREEN_WIDTH){
			x = SCREEN_WIDTH - BALL_RADIUS;
			speed_x = -speed_x;
		}
		else if (x < 0){
			x = 0;
			speed_x = -speed_x;
		}
		
		//check y axis
		if (y > SCREEN_HEIGHT){
			y = SCREEN_HEIGHT - BALL_RADIUS;
			speed_y = -speed_y;
		}
		else if (y < 0){
			y = 0;
			speed_y = -speed_y;
		}
	}
	
	private:
	uint16_t old_x;
	uint16_t old_y;
};