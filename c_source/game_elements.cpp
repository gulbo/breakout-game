#include <stdint.h>
#include "GLCD.h"
#include "LPC17xx.h"
#include "DAC_LPC1768.h"

#define BRICK_LENGTH 23		//20
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
#define PADDLE_ANGLE 0.1 						//10% of the paddle
#define INCREMENT_BALL_SPEED 1.2
#define DECREMENT_BALL_SPEED 0.8

#define BALL_COLOUR White
#define BALL_DIAMETER 4
#define BALL_MAX_SPEED 2
#define BACKGROUND_COLOUR Black

struct Paddle{
	uint16_t x;
	uint16_t length;
	bool going_left;
	bool going_right;
	
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
		going_left = false;
		going_right = false;
	}
	
	//default constructor in EASY difficulty
	Paddle() {length = 60; x = 90;}
	
	void draw(){
		GLCD_DrawRect(PADDLE_Y,x_drawn,PADDLE_WIDTH,length,BACKGROUND_COLOUR);
		GLCD_DrawRect(PADDLE_Y,x,PADDLE_WIDTH,length,PADDLE_COLOUR);
		x_drawn = x;
	}

	void move(bool direction_left, bool direction_right){
		if (direction_left == 1 && direction_right == 0){	//means right click																						
			if (x == 0);																		//do nothing instead of moving the paddle
			else{		
				going_left = true;
				going_right = false;
				x -= PADDLE_SPEED;
			}
		}
		else if (direction_left == 0 && direction_right == 1){	//means left click
			if (x + length == SCREEN_HEIGHT);								//do nothing instead of moving the paddle
			else{
				going_left = false;
				going_right = true;
				x += PADDLE_SPEED;
			}
		}
		else{
			going_left = false;
			going_right = false;
		}
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
	uint16_t x_drawn;
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
	Brick() {hit = true; drawn = true; x = 0; y = 0;} //set drawn and hit, otherwise unexpected behaviour could occour (drawing or hitting it)
	
	//destructor (in case we need DELETE)
	//~Brick(){ }
	
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
	int number_of_bricks;
	
	//constructor
	Ball(double x, double y){
		this->x = x;
		this->y = y;
		speed_x = 0.5;
		speed_y = 1.1;
		number_of_bricks = 70;
	}
	
	void draw(){
		GLCD_DrawRect(y_drawn,x_drawn,BALL_DIAMETER,BALL_DIAMETER,BACKGROUND_COLOUR);
		GLCD_DrawRect(y,x,BALL_DIAMETER,BALL_DIAMETER,BALL_COLOUR);
		x_drawn = x;
		y_drawn = y;
	}
	
	//uses speed parameters to move the ball inside the screen
	//bounces when hits a border
	void move(Paddle p){
		old_x = x;
		old_y = y;
		
		if (speed_y > BALL_MAX_SPEED) 
			speed_y = BALL_MAX_SPEED;
		
		x += speed_x;
		y += speed_y;
		
		// set ball borders
		ball_top = y + BALL_DIAMETER;
		ball_bottom = y;											
		ball_left = x;
		ball_right = x + BALL_DIAMETER;
		
		//check x axis
		if (ball_right >= SCREEN_HEIGHT){ //right screen
			x = SCREEN_HEIGHT - BALL_DIAMETER;
			speed_x = -speed_x;
		}
		else if (ball_left <= 0){ //left screen
			x = 0;
			speed_x = -speed_x;
		}
		
		//check y axis
		if (ball_top >= SCREEN_WIDTH){ //top screen
			y = SCREEN_WIDTH - BALL_DIAMETER;
			speed_y = -speed_y;
		}
		//check if hits the paddle
		else if (ball_bottom <= PADDLE_WIDTH+PADDLE_Y	&& ( ball_right>=p.x && ball_left<=(p.x+p.length))){
			y = PADDLE_WIDTH+PADDLE_Y; //put again on top of the paddle
			double left_angle = p.x + p.length * PADDLE_ANGLE;
			double right_angle = p.x + p.length * (1-PADDLE_ANGLE);
			//if the paddle goes in the opposit direction of the ball...
			if((p.going_left==true && speed_x<0) || (p.going_right==true && speed_x>0))
				speed_y = -DECREMENT_BALL_SPEED * speed_y;	//decrease the speed
			//... or if it goes in the same direction
			else if((p.going_left==true && speed_x>0) || (p.going_right==true && speed_x<0))
				speed_y = -INCREMENT_BALL_SPEED * speed_y;	//increase the speed
			else 
				speed_y = -speed_y;
			//check the angles
			if (ball_right <= left_angle){ //force ball to go to LEFT
				if (speed_x > 0)
					speed_x = -speed_x;
			}
			else if (ball_left >= right_angle){ //force ball to go to RIGHT
				if (speed_x < 0)
					speed_x = -speed_x;
			}
		}
		else if(ball_bottom < PADDLE_WIDTH+PADDLE_Y && ball_right == p.x)
			speed_x = -speed_x;
		else if(ball_bottom < PADDLE_WIDTH+PADDLE_Y && ball_left == (p.x+p.length))
			speed_x = -speed_x;
		else if (y<=0){ //bottom screen YOU LOST
			play_music(LOSS,LOSS_DURATION);
			system_reset();
		}

		if(number_of_bricks==0){ //YOU WON
			play_music(WIN,WIN_DURATION);
			system_reset();
		}
	}
	
	/* void check_collision(Brick brick){													
		if(!brick.hit){
			if (collision_up(brick)){
				// the ball is coming from the BOTTOM
				brick.hit = true;
				brick.drawn = false;
				speed_y = -speed_y;
				brick.draw();
			}
			else if (collision_down(brick)){
				// the ball is coming from the TOP
				brick.hit = true;
				brick.drawn = false;
				speed_y = -speed_y;
				//brick.draw();
				
			}
			else if (collision_right(brick)){
				// the ball is coming from LEFT
				brick.hit = true;
				brick.drawn = false;
				speed_x = -speed_x;
				//brick.draw();
			}
			else if (collision_left(brick)){
				// the ball is coming from the RIGHT
				brick.hit = true;
				brick.drawn = false;
				speed_x = -speed_x;
				//brick.draw();
			}
		}
	} 
	*/
	
	bool check_collision_new(Brick brick){
		bool is = false;
		if(!brick.hit){
			double ball_center_x = x + BALL_DIAMETER/2.0;
			double ball_center_y = y + BALL_DIAMETER/2.0;
			
			uint8_t going_up = 0;
			uint8_t going_dw = 0;
			uint8_t going_lx = 0;
			uint8_t going_rx = 0;
			
			
			
			if (is_inside(ball_left,ball_top,brick)){	//LEFT-TOP
				going_up++;
				going_lx++;
			}
			if (is_inside(ball_center_x,ball_top,brick)){ //CENTER-TOP
				going_up++;
			} 
			if (is_inside(ball_right,ball_top,brick)){ //right-TOP
				going_up++;
				going_rx++;
			}
			if (is_inside(ball_left,ball_center_y,brick)){ //LEFT-CENTER
				going_lx++;
			} 
			if (is_inside(ball_right,ball_center_y,brick)){ //RIGHT-CENTER
				going_rx++;
			}
			if (is_inside(ball_left,ball_bottom,brick)){ //LEFT-BOTTOM
				going_lx++;
				going_dw++;
			} 
			if (is_inside(ball_center_x,ball_bottom,brick)){ //CENTER-BOTTOM
				going_dw++;
			} 
			if (is_inside(ball_right,ball_bottom,brick)){ //RIGHT-BOTTOM
				going_rx++;
				going_dw++;
			}
			
			if (going_up >= 2){
				// the ball is coming from BOTTOM
				brick.hit = true;
				brick.drawn = false;
				speed_y = -speed_y;
				brick.draw();
				is = true;
				number_of_bricks--;
			}
			else if (going_dw >= 2){
				// the ball is coming from TOP
				brick.hit = true;
				brick.drawn = false;
				speed_y = -speed_y;
				brick.draw();
				is = true;
				number_of_bricks--;
			}
			else if (going_rx >= 2){
				// the ball is coming from LEFT
				brick.hit = true;
				brick.drawn = false;
				speed_x = -speed_x;
				brick.draw();
				is = true;
				number_of_bricks--;
			}
			else if (going_lx >= 2){
				// the ball is coming from RIGHT
				brick.hit = true;
				brick.drawn = false;
				speed_x = -speed_x;
				brick.draw();
				is = true;
				number_of_bricks--;
			}
			
			if (is)
				play_sound();
		}
		return is;
	}
	
	//TO REST THE SYSTEM...REGISTERS TAKEN FROM THE LPC DATASHEET
	void system_reset(){
		SCB->AIRCR = (0x5FA<<SCB_AIRCR_VECTKEY_Pos)|SCB_AIRCR_SYSRESETREQ_Msk;		//writes 0x5FA in VECTKEY field [31:16] and set SYSRESETREQ to 1
		for(;;);
	}
		
	private:
		double x_drawn;
		double y_drawn;
	  double ball_top;
		double ball_bottom;													
		double ball_right;
		double ball_left;
		
		//check if (x,y) is inside a brick
		bool is_inside(double x, double y, Brick brick){
			if (x >= brick.x && x <= brick.x+BRICK_LENGTH)
				if (y >= brick.y && y < brick.y+BRICK_WIDTH)
					return true;
			return false;
		}
		
		//from the pow of the ball, check if the upper side of the ball is inside a brick
		bool collision_up(Brick brick){
			if (brick.y <= ball_top)
				if (brick.y+BRICK_WIDTH >= ball_top)
					if (brick.x <= ball_right)
						if (brick.x+BRICK_LENGTH >= ball_left)
							return true;
			return false;
		}
		
		bool collision_down(Brick brick){
			if (brick.y <= ball_bottom)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_right)
						if (ball_left <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
		
		bool collision_left(Brick brick){
			if (brick.y <= ball_top)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_left)
						if (ball_left <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
		
		bool collision_right(Brick brick){
			if (brick.y <= ball_top)
				if (ball_bottom <= brick.y+BRICK_WIDTH)
					if (brick.x <= ball_right)
						if (ball_right <= brick.x+BRICK_LENGTH)
							return true;
			return false;
		}
};
