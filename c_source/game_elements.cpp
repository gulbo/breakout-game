#include <stdint.h>
#include "GLCD.h"
#include "LPC17xx.h"

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
			case 1: //easy level
				length = 60;
				x = 90;
				break;
			case 2: //medium
				length = 40;
				x = 100;
				break;
			case 3: //hard
				length = 20;
				x = 110;
				break;
		}
	}
	
	//default constructor in EASY difficulty
	Paddle() {length = 60; x = 90;}
	
	void draw(){
		GLCD_DrawRect(PADDLE_Y,old_x,PADDLE_WIDTH,length,BACKGROUND_COLOUR);
		GLCD_DrawRect(PADDLE_Y,x,PADDLE_WIDTH,length,PADDLE_COLOUR); 
	}

	void move(bool direction_left, bool direction_right){
		if (direction_left==1 && direction_right==0){							//means right click
			if (x == 0);																							//do nothing instead of moving the paddle
			else{		
				old_x = x + PADDLE_WIDTH;
				x -= 1;
			}
		}
		else if (direction_left==0 && direction_right==1){				//means left click
			if (x + length == SCREEN_HEIGHT);												//do nothing instead of moving the paddle
			else{
				old_x = x;
				x += 1;
			}
		}
		else;
	}
	
	void set_difficulty(uint8_t diff){
		switch(diff){
			case 1: //easy level
				length = 60;
				x = 90;
				break;
			case 2: //medium
				length = 40;
				x = 100;
				break;
			case 3: //hard
				length = 20;
				x = 110;
				break;
		}
	}
	
	
	private:
	uint16_t old_x;
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
	
	//default constructor 
	//Brick() {hit = true; drawn = true;} //set drawn and hit, otherwise unexpected behaviour could occour (drawing or hitting it)
	
	//destructor (in case we need DELETE)
	~Brick(){ }
	
	void draw(){
		if (!drawn){
			if (hit)
				GLCD_DrawRect(y,x,BRICK_WIDTH,BRICK_LENGTH,BACKGROUND_COLOUR);
			else
				GLCD_DrawRect(y,x,BRICK_WIDTH,BRICK_LENGTH,colour);
			drawn = true;
		}
	}

};

struct Ball{
	double x;											//DOUBLE TO BE ABLE TO SET THE SPEED TO 0.1, 0.2, ETC...
	double y;
	double old_x;
	double old_y;
	double speed_x;
	double speed_y;
	
	//constructor
	Ball(double x, double y){
		this->x = x;
		this->y = y;
		speed_x = 0.1;
		speed_y = 0.1;
	}
	void draw(){
		GLCD_DrawRect(y_drawn,x_drawn,BALL_DIAMETER,BALL_DIAMETER,BACKGROUND_COLOUR);
		GLCD_DrawRect(y,x,BALL_DIAMETER,BALL_DIAMETER,BALL_COLOUR);
		x_drawn = x;
		y_drawn = y;
	}
	
	/////////////////////////////MODIFIED EXPRESSIONS TO MAKE IT WORK//////////////////////////////////
	/*inside the if() use >=,<= instead of == to be allowed to use different values of speed_x, speed_y instead of 0.5 and 1*/
	
	//uses speed parameters to move the ball inside the screen
	//bounces when hits a border
	void move(Paddle p){
		old_x = x;
		old_y = y;
		x += speed_x;
		y += speed_y;
		
		// by now we assume that we have only 45 deg angles
		//check x axis
		if (x+BALL_DIAMETER>=SCREEN_HEIGHT){
			x = SCREEN_HEIGHT - BALL_DIAMETER;
			speed_x = -speed_x;
		}
		else if (x<=0){
			x = 0;
			speed_x = -speed_x;
		}
		
		//check y axis
		if (y+BALL_DIAMETER>=SCREEN_WIDTH){
			y = SCREEN_WIDTH - BALL_DIAMETER;
			speed_y = -speed_y;
		}
		else if (y<=PADDLE_WIDTH+PADDLE_Y&&(x>=p.x&&x<=(p.x+p.length))){
			y = PADDLE_WIDTH+PADDLE_Y;
			speed_y = -speed_y;
		}
		else if (y<=0){
			system_reset();
		}
	}
	
	void check_collision(Brick brick){
		double ball_top = y + BALL_DIAMETER;
		double ball_bottom = y;													
		double ball_right = x + BALL_DIAMETER;
		double ball_left = x;														

		//if ((ball_left>=brick.x || ball_right<=brick.x+BRICK_LENGTH) && (ball_top>=brick.y)){
		if (collision_up(brick)){
			// the ball is coming from the BOTTOM
			brick.hit = true;
			brick.drawn = false;
			speed_y = -speed_y;
		}
		else if (collision_down(brick)){
			// the ball is coming from the TOP
			brick.hit = true;
			brick.drawn = false;
			speed_y = -speed_y;
		}
		else if (collision_right(brick)){
			// the ball is coming from LEFT
			brick.hit = true;
			brick.drawn = false;
			speed_x = -speed_x;
		}
		else if (collision_left(brick)){
			// the ball is coming from the RIGHT
			brick.hit = true;
			brick.drawn = false;
			speed_x = -speed_x;
		}
	}

	private:
		double x_drawn;
		double y_drawn;
		//TO REST THE SYSTEM...REGISTERS TAKEN FROM THE LPC DATASHEET
		void system_reset(){
			SCB->AIRCR = (0x5FA<<SCB_AIRCR_VECTKEY_Pos)|SCB_AIRCR_SYSRESETREQ_Msk;		//writes 0x5FA in VECTKEY field [31:16] and set SYSRESETREQ to 1
			for(;;);
		}
		
		//from the pow of the ball, check if the upper side of the ball is inside a brick
		bool collision_up(Brick brick){
			double ball_top = y + BALL_DIAMETER;
			double ball_bottom = y;													
			double ball_right = x + BALL_DIAMETER;
			double ball_left = x;
			
			if (brick.y <= ball_top)
				if (brick.y+BRICK_WIDTH >= ball_top)
					if (brick.x <= ball_right)
						if (brick.x+BRICK_LENGTH >= ball_left)
							return true;
			return false;
		}
		
		bool collision_down(Brick brick){
			double ball_top = y + BALL_DIAMETER;
			double ball_bottom = y;													
			double ball_right = x + BALL_DIAMETER;
			double ball_left = x;
			
			if (brick.y <= ball_bottom)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_right)
						if (ball_left <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
		
		bool collision_left(Brick brick){
			double ball_top = y + BALL_DIAMETER;
			double ball_bottom = y;													
			double ball_right = x + BALL_DIAMETER;
			double ball_left = x;
			
			if (brick.y <= ball_top)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_left)
						if (ball_left <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
		
		bool collision_right(Brick brick){
			double ball_top = y + BALL_DIAMETER;
			double ball_bottom = y;													
			double ball_right = x + BALL_DIAMETER;
			double ball_left = x;
			
			if (brick.y <= ball_top)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_right)
						if (ball_right <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
};