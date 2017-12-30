#include "GLCD.h"
#include "Board_Buttons.h"
#include "Board_Led.h"
#include <stdio.h>
#include "game_elements.cpp"
#include "cmsis_os.h"
#include <vector>

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
int8_t difficulty = -1;
Paddle p(1);
Ball ball(POSITION_TO_CENTRE_BALL,BEGINNING_OF_FALLING_BALL);

void GameInitialization(){
	
	GLCD_Clear(Black);
	/////////////////////////////NEW//////////////////////////////////
	while(difficulty==-1){																														//difficulty not selected. As soon as it is selected, the game starts
		if(Button_GetState(0)==1&&Button_GetState(1)==0&&Button_GetState(2)==0)					//BUTTON1 for easy
			difficulty=1;
		else if(Button_GetState(0)==0&&Button_GetState(1)==1&&Button_GetState(2)==0)		//BUTTON2 for medium
			difficulty=2;
		else if(Button_GetState(0)==0&&Button_GetState(1)==0&&Button_GetState(2)==1)		//BUTTON3 for hard
			difficulty=3;
		else;
	}
	Paddle p_temp(difficulty);
	p = p_temp;
	p.draw();
	
	int i=2;
	int j=0;
	while(i*j<=END_OF_LINES){		
		Brick b_ji(i,BRICK_LINE_HEIGHT_BEGINNING-10*j);
		b_ji.draw();
		i+=24;
		if(i==BRICK_LINE_LENGTH_END){
			i=2;
			j++;
		}
	}
	
	Brick b_last(LAST_BRICK,BRICK_LINE_HEIGHT_BEGINNING-10*j);
	b_last.draw();

	/////////////////////////////NEW//////////////////////////////////
	while(ball.y>(PADDLE_WIDTH+PADDLE_Y)){
		ball.old_y = ball.y+1;
		ball.old_x = ball.x;
		ball.draw();
		ball.y--;
		delay(1);
	}
}

/////////////////////////////NEW//////////////////////////////////
void game(void const *argument){										//TODO (PASS PARAMETERS)
  for(;;){
		ball.move(p);
		direction_left = Button_GetState(0);
		direction_right = Button_GetState(1);
		p.move(direction_left,direction_right);
		osDelay(GAME_DELAY);
  }
}

/////////////////////////////NEW//////////////////////////////////
void refresh_screen(void const *argument){					//TODO (PASS PARAMETERS)
  for(;;){
		ball.draw();
		p.draw();
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
