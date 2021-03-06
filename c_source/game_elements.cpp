#include <stdint.h>
#include "GLCD.h"
#include "LPC17xx.h"
#include "DAC_LPC1768.h"
#include "cmsis_os.h"

#define BRICK_LENGTH 23		
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
#define PADDLE_SPEED 1.5
#define PADDLE_ANGLE 0.1 //10% of the paddle
#define PADDLE_EASY 60
#define PADDLE_MEDIUM 40
#define PADDLE_HARD 20
#define INCREMENT_BALL_SPEED 1.2
#define DECREMENT_BALL_SPEED 0.8

#define BALL_COLOUR White
#define BALL_DIAMETER 4
#define BALL_MAX_SPEED 2
#define BACKGROUND_COLOUR Black

struct Paddle{
	float x;
	uint16_t length;
	bool going_left;
	bool going_right;
	bool self; 			//enables auto-mode, for debug purposes
	
	//constructor, easy default mode, use init() instead
	Paddle() {
		//self = false;
		//set_difficulty(1);
		//going_left = false;
		//going_right = false;
		//rand_point = 0;
	}
	
	/*************************Method to initialize the paddle*************************/
	void init(uint8_t diff){
		self = false;
		set_difficulty(diff);
		going_left = false;
		going_right = false;
		rand_point = osKernelSysTick()%length;
	}
	
	/*************************Method to draw the paddle on the screen*************************/
	void draw(){
		GLCD_DrawRect(PADDLE_Y,x_drawn,PADDLE_WIDTH,length,BACKGROUND_COLOUR);	//cancels the paddle of the previous cycle		
		GLCD_DrawRect(PADDLE_Y,x,PADDLE_WIDTH,length,PADDLE_COLOUR);		//displays the paddle after its movement
		x_drawn = x;
	}

	/*************************Method to make the paddle move*************************/
	void move(bool direction_left, bool direction_right, int x_ball=-1){
		if(x_ball!=-1 && self){							//automatic mode
			x = x_ball-rand_point;
			if (x <= 0)
				x = 0;
			else if (x+length > SCREEN_HEIGHT)
				x = SCREEN_HEIGHT-length;
		}
		if (direction_left == 1 && direction_right == 0){		//means left click																						
			if (x <= 0){				//paddle has reached the left side of the screen...do nothing instead of moving the paddle
				going_left = false;
				going_right = false;
				x = 0;
			}
			else{		
				going_left = true;
				going_right = false;
				x -= PADDLE_SPEED;//*0.65;		//GPIO read problem, coefficient to fix it
			}
		}
		else if (direction_left == 0 && direction_right == 1){	//means right click
			if (x + length >= SCREEN_HEIGHT){				//paddle has reached the right side of the screen...do nothing instead of moving the paddle
				going_left = false;																		//in case both buttons were pressed
				going_right = false;
				x = SCREEN_HEIGHT-length;
			}
			else{
				going_left = false;
				going_right = true;
				x += PADDLE_SPEED;
			}
		}
		else{																										//in all other cases, paddle does not move! This to avoid problems
			going_left = false;																		//in case both buttons were pressed
			going_right = false;
		}
	}
	
	void set_difficulty(uint8_t diff){
		switch(diff){
			case 1: 								//easy level
				length = PADDLE_EASY;
				x = (SCREEN_HEIGHT-PADDLE_EASY)/2;
				break;
			case 2: 								//medium
				length = PADDLE_MEDIUM;
				x = (SCREEN_HEIGHT-PADDLE_MEDIUM)/2;
				break;
			case 3: 								//hard
				length = PADDLE_HARD;
				x = (SCREEN_HEIGHT-PADDLE_HARD)/2;
				break;
			case 4:								//auto
				length = PADDLE_MEDIUM;
				x = (SCREEN_HEIGHT-PADDLE_MEDIUM)/2;
				self = true;
				break;
			default:								//error
				length = 0;
				x = -1;
		}
	}
	
	private:
	uint16_t x_drawn;
	uint16_t rand_point; 	//debug, rand_point between 0 and length
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
	Brick() {/*hit = true; drawn = true; x = 0; y = 0*/;} //set drawn and hit, otherwise unexpected behaviour could occour (drawing or hitting it)
	
	/*************************Method to set Bricks after cration*************************/	
	void set(uint16_t x, int16_t y){
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
	
	/*************************Method to draw the bricks*************************/
	void draw(){
		if (!drawn){						
			if (hit)
				GLCD_DrawRect(y,x,BRICK_WIDTH,BRICK_LENGTH,BACKGROUND_COLOUR);	//if the ball hits it, it is "deleted" (i.e. coloured by the background colour
			else
				GLCD_DrawRect(y,x,BRICK_WIDTH,BRICK_LENGTH,colour);			//if not, it is displayed
			drawn = true;
		}
	}
};

struct Ball{
	float x;											
	float y;
	float old_x;
	float old_y;
	float speed_x;
	float speed_y;
	
	//constructor
	Ball(){;}
		
	/*************************Method to initialize the ball*******************/
	void init(float x, float y){
		this->x = x;
		this->y = y;
		speed_x = 0.5;
		speed_y = 1.1;
	}
	
	/*************************Method to draw the ball*************************/
	void draw(){
		GLCD_DrawRect(y_drawn,x_drawn,BALL_DIAMETER,BALL_DIAMETER,BACKGROUND_COLOUR); //deletes the ball of the previous cycle, before its movement
		GLCD_DrawRect(y,x,BALL_DIAMETER,BALL_DIAMETER,BALL_COLOUR);				//displays the ball after its movement
		x_drawn = x;
		y_drawn = y;
	}
	
	//uses speed parameters to move the ball inside the screen
	void move(const Paddle& p){
		old_x = x;
		old_y = y;
		
		if (speed_y > BALL_MAX_SPEED) 
			speed_y = BALL_MAX_SPEED;			//to avoid uncorrect behaviours of the ball
		
		x += speed_x;
		y += speed_y;
		
		// set ball borders
		ball_top = y + BALL_DIAMETER;
		ball_bottom = y;											
		ball_left = x;
		ball_right = x + BALL_DIAMETER;
		
		//check x axis
		if (ball_right >= SCREEN_HEIGHT){ 			//right border of the screen
			x = SCREEN_HEIGHT - BALL_DIAMETER;
			speed_x = -speed_x;				//bounces
		}
		else if (ball_left <= 0){ 				//left border of the screen
			x = 0;
			speed_x = -speed_x;				//bounces
		}
		
		//check y axis
		if (ball_top >= SCREEN_WIDTH){ 			//top border of the screen
			y = SCREEN_WIDTH - BALL_DIAMETER;
			speed_y = -speed_y;				//bounces
		}
		
		//check if hits the paddle
		else if (ball_bottom <= PADDLE_WIDTH+PADDLE_Y	&& ( ball_right>=p.x && ball_left<=(p.x+p.length))){
			y = PADDLE_WIDTH+PADDLE_Y; 											//put the ball again on top of the paddle			
			//computes the angles of the paddle
			float left_angle = p.x + p.length * PADDLE_ANGLE;																	
			float right_angle = p.x + p.length * (1-PADDLE_ANGLE);
			
			//if the paddle goes in the opposit direction of the ball...
			if((p.going_left==true && speed_x<0) || (p.going_right==true && speed_x>0))
				speed_y = -DECREMENT_BALL_SPEED * speed_y;						//decreases the speed
			
			//... or if it goes in the same direction
			else if((p.going_left==true && speed_x>0) || (p.going_right==true && speed_x<0))
				speed_y = -INCREMENT_BALL_SPEED * speed_y;						//increase the speed
			else 
				speed_y = -speed_y;										//just bounces on the paddle
			
			//check the angles
			if (ball_right <= left_angle ){ 							//forces ball to go to LEFT
				if (speed_x > 0)
					speed_x = -speed_x;
			}
			else if (ball_left >= right_angle){ 							//force ball to go to RIGHT
				if (speed_x < 0)
					speed_x = -speed_x;
			}
		}		
	}
	
	/*************************Method to check the collision between ball and bricks*************************/
	bool check_collision(Brick& brick){
		bool is = false;																											//by default...nobody has hit anything
		if(!brick.hit){
			float ball_center_x = x + BALL_DIAMETER/2.0;												
			float ball_center_y = y + BALL_DIAMETER/2.0;
			
			uint8_t going_up = 0;
			uint8_t going_dw = 0;
			uint8_t going_lx = 0;
			uint8_t going_rx = 0;
			
			
			
			if (is_inside(ball_left,ball_top,brick)){				//LEFT-TOP "sensor"
				going_up++;
				going_lx++;
			}
			if (is_inside(ball_center_x,ball_top,brick)){ 			//CENTER-TOP "sensor"
				going_up++;
			} 
			if (is_inside(ball_right,ball_top,brick)){ 			//RIGHT-TOP "sensor"
				going_up++;
				going_rx++;
			}
			if (is_inside(ball_left,ball_center_y,brick)){ 			//LEFT-CENTER "sensor"
				going_lx++;
			} 
			if (is_inside(ball_right,ball_center_y,brick)){ 		//RIGHT-CENTER "sensor"
				going_rx++;
			}
			if (is_inside(ball_left,ball_bottom,brick)){ 			//LEFT-BOTTOM "sensor"
				going_lx++;
				going_dw++;
			} 
			if (is_inside(ball_center_x,ball_bottom,brick)){ 		//CENTER-BOTTOM "sensor"
				going_dw++;
			} 
			if (is_inside(ball_right,ball_bottom,brick)){ 			//RIGHT-BOTTOM "sensor"
				going_rx++;
				going_dw++;
			}
			
			if (going_up >= 2){																									//if 2 sensors touch a brick
				// the ball is coming from BOTTOM
				brick.hit = true;																									//brick has ben hit
				brick.drawn = false;																							//to make it become black
				speed_y = -speed_y;																								//bounces
				is = true;
			}
			else if (going_dw >= 2){
				// the ball is coming from TOP
				brick.hit = true;
				brick.drawn = false;
				speed_y = -speed_y;
				is = true;
			}
			else if (going_rx >= 2){
				// the ball is coming from LEFT
				brick.hit = true;
				brick.drawn = false;
				speed_x = -speed_x;
				is = true;
			}
			else if (going_lx >= 2){
				// the ball is coming from RIGHT
				brick.hit = true;
				brick.drawn = false;
				speed_x = -speed_x;
				is = true;
			}
			
			if (is)
				play_sound();																											//play a note if a brick is hit
		}
		return is;
	}
		
	private:
		float x_drawn;
		float y_drawn;
		float ball_top;
		float ball_bottom;													
		float ball_right;
		float ball_left;
		
		//checks if (x,y) is inside a brick
		bool is_inside(float x, float y, const Brick& brick){
			if (x >= brick.x && x <= brick.x+BRICK_LENGTH)
				if (y >= brick.y && y < brick.y+BRICK_WIDTH)
					return true;
			return false;
		}
		
		/***OLD FUNCTIONS, NOT USED ***
		//from the point of view of the ball, checks if the upper side of the ball is inside a brick
		bool collision_up(const Brick& brick){
			if (brick.y <= ball_top)
				if (brick.y+BRICK_WIDTH >= ball_top)
					if (brick.x <= ball_right)
						if (brick.x+BRICK_LENGTH >= ball_left)
							return true;
			return false;
		}
		//from the point of view of the ball, checks if the lower side of the ball is inside a brick
		bool collision_down(const Brick& brick){
			if (brick.y <= ball_bottom)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_right)
						if (ball_left <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
		//from the point of view of the ball, checks if the left side of the ball is inside a brick
		bool collision_left(const Brick& brick){
			if (brick.y <= ball_top)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_left)
						if (ball_left <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
		//from the point of view of the ball, checks if the right side of the ball is inside a brick
		bool collision_right(const Brick& brick){
			if (brick.y <= ball_top)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_right)
						if (ball_right <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
		*/
};
