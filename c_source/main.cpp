#include "GLCD.h"
#include "Board_Buttons.h"
#include "Board_Led.h"
#include <stdio.h>
#include "game_elements.cpp"
#include "cmsis_os.h"
#include "LPC17xx.h"

#define BRICK_LINE_HEIGHT_BEGINNING 270
#define END_OF_LINES 1210								//add 242 to increase the #rows by 1, subtract 242 to decrease the #rows by 1.......CURRENTLY 7 ROWS
#define BRICK_LINE_LENGTH_END	242
#define LAST_BRICK	218
#define POSITION_TO_CENTRE_BALL 117
#define BEGINNING_OF_FALLING_BALL 100
#define SCREEN_DELAY 30 								//30ms -> 33Hz should be fine to avoid lag
#define GAME_DELAY		5									//TODO find the correct value

//RTOS threads
osThreadId tid_screen;    //thread for the refresh of the screen
osThreadId tid_game;			//thread for the game execution

//global objects/variables to be used in different methods
uint32_t direction_left, direction_right;
Paddle pad;
Ball ball(POSITION_TO_CENTRE_BALL,BEGINNING_OF_FALLING_BALL);
Brick* bricks_array[70];
Brick b_last(220,BRICK_LINE_HEIGHT_BEGINNING-60);

inline bool button1_click(){
	if (Button_GetState(0) == 1 && Button_GetState(1) == 0 && Button_GetState(2) == 0)
		return true;
	else
		return false;
}

inline bool button2_click(){
	if (Button_GetState(0) == 0 && Button_GetState(1) == 1 && Button_GetState(2) == 0)
		return true;
	else
		return false;
}

inline bool button3_click(){
	if (Button_GetState(0) == 0 && Button_GetState(1) == 0 && Button_GetState(2) == 1)
		return true;
	else
		return false;
}
void GameInitialization(){
	int8_t difficulty = -1;
	GLCD_Clear(Black);
	/////////////////////////////NEW button1_click()//////////////////////////////////
	while(difficulty==-1){																														//difficulty not selected. As soon as it is selected, the game starts
		if(button1_click())					//BUTTON1 for easy
			difficulty=1;
		else if(button2_click())		//BUTTON2 for medium
			difficulty=2;
		else if(button3_click())		//BUTTON3 for hard
			difficulty=3;
		else;
	}
	
	pad.set_difficulty(difficulty);
	
	int i=2;
	int j=0;
	int s=0;
	/////////////////////////////NEW bricks_array//////////////////////////////////
	while(i*j<=END_OF_LINES){
		bricks_array[s] = new Brick(i,BRICK_LINE_HEIGHT_BEGINNING-10*j); //DYNAMIC ALLOCATION
		i+=24;
		s++;
		if(i==BRICK_LINE_LENGTH_END){
			i=2;
			j++;
		}
	}
	
	//Brick b_last(LAST_BRICK,BRICK_LINE_HEIGHT_BEGINNING-10*j);

	while(ball.y>(PADDLE_WIDTH+PADDLE_Y)){
		ball.old_y = ball.y+1;
		ball.old_x = ball.x;
		ball.y--;
		delay(1);
	}
}

void game(void const *argument){										//TODO (PASS PARAMETERS)
  for(;;){
		ball.move(pad);
		int i;
		for(i=0; i<70; i++)
			ball.check_collision(*bricks_array[i]);
		ball.check_collision(b_last);
		direction_left = Button_GetState(0);
		direction_right = Button_GetState(1);
		pad.move(direction_left,direction_right);
		osDelay(GAME_DELAY);
  }
}

void refresh_screen(void const *argument){					//TODO (PASS PARAMETERS)
  for(;;){
		b_last.draw();
		ball.draw();
		pad.draw();
		int i;
		/*for(i=0; i<70; i++)
			bricks_array[i]->draw();*/
    osDelay(SCREEN_DELAY);
  }
}


osThreadDef(game, osPriorityNormal, 1, 0);
osThreadDef(refresh_screen, osPriorityNormal, 1, 0);
	
int main(void){
  if (osKernelInitialize () != osOK){  //initialize the RTOS
    //exit with an error message
		return -1;
  }
 
	GLCD_Init();
	Buttons_Initialize();
	GameInitialization();
	
	tid_game = osThreadCreate(osThread(game), NULL);
	if (tid_game == NULL) {
		//Failed to create the thread
  }
		
	tid_screen = osThreadCreate(osThread(refresh_screen), NULL);
	if (tid_screen == NULL) {  
		//Failed to create the thread
  }

	if (osKernelStart() != osOK){ //start the kernel
		// kernel could not be started
		return -1;
	}
	
	osDelay(osWaitForever); //main thread waits forever
	
	//NO MAN'S LAND
	while(1);
	
	return -1;
}
