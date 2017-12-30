#include "GLCD.h"
#include "Board_Buttons.h"
#include "Board_Led.h"
#include <stdio.h>
#include "game_elements.cpp"
#include "cmsis_os.h"

#define BRICK_LINE_HEIGHT_BEGINNING 270
#define END_OF_LINES 1210								//add 242 to increase the #rows by 1, subtract 242 to decrease the #rows by 1.......CURRENTLY 7 ROWS
#define BRICK_lINE_LENGTH_END	242
#define LAST_BRICK	218
#define POSITION_TO_CENTRE_BALL 117
#define BEGINNING_OF_FALLING_BALL 100
#define SCREEN_DELAY 30 								//30ms -> 33Hz should be fine to avoid lag
#define GAME_DELAY		5									//TODO find the correct value


//RTOS threads
osThreadId tid_screen;    //thread for the refresh of the screen
osThreadId tid_game;			//thread for the game execution

//global object of type Ball

void GameInitialization(unsigned int difficulty){
	
	GLCD_Clear(Black);
	int i=2;
	int j=0;
	short brick_color;
	while(i*j<=END_OF_LINES){		
		
		Brick b (i,BRICK_LINE_HEIGHT_BEGINNING-10*j);
		b.draw();
		i+=24;
		if(i==BRICK_lINE_LENGTH_END){
			i=2;
			j++;
		}
	}
	Brick b_i(LAST_BRICK,BRICK_LINE_HEIGHT_BEGINNING-10*j);
	b_i.draw();
	
	Paddle p(difficulty);
	p.draw();
	
	Ball ball(POSITION_TO_CENTRE_BALL,BEGINNING_OF_FALLING_BALL);
	while(ball.y>(PADDLE_WIDTH+PADDLE_Y)){
		ball.draw();
		ball.y--;
		delay(1);
	}
	while(1){
		ball.move();
		ball.draw();
		delay(2);
	}
}


void game(void const *argument){
  for(;;){
		osDelay(GAME_DELAY);
  }
}


void refresh_screen(void const *argument){
  for(;;){
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
	LED_Initialize();
	GameInitialization(1);
	
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
