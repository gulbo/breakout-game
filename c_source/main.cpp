#include "GLCD.h"
#include "Board_Buttons.h"
#include "Board_Led.h"
#include <stdio.h>
#include "game_elements.cpp"
#include "cmsis_os.h"

#define SCREEN_DELAY 30 	//30ms -> 33Hz should be fine to avoid lag
#define GAME_DELAY		5		//TODO find the correct value


//RTOS threads
osThreadId tid_screen;    //thread for the refresh of the screen
osThreadId tid_game;			//thread for the game execution
	
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

void game(void const *argument){
  for(;;){
		//code.....
		
		osDelay(GAME_DELAY);
  }
}

void refresh_screen(void const *argument){
  for(;;){
		//code.....
		
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
