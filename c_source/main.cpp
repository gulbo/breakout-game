#include "GLCD.h"
#include "Board_Buttons.h"
#include <stdio.h>
#include "game_elements.cpp"
#include "cmsis_os.h"
#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "DAC_LPC1768.h"

#define BRICK_LINE_HEIGHT_BEGINNING 270
#define END_OF_LINES 1210							//add 242 to increase the #rows by 1, subtract 242 to decrease the #rows by 1.......CURRENTLY 7 ROWS
#define BRICK_LINE_LENGTH_END	242
#define LAST_BRICK	218
#define POSITION_TO_CENTRE_BALL 117
#define BEGINNING_OF_FALLING_BALL 100
#define SCREEN_DELAY 10 								//30ms -> 33Hz should be fine to avoid lag
#define GAME_DELAY		5									//TODO find the correct value

//RTOS threads
osThreadId tid_screen;    //thread for the refresh of the screen
osThreadId tid_game;			//thread for the game execution

osMutexDef (game_mutex);    // Declare mutex
osMutexId game_mutex_id; // Mutex ID

//global objects/variables to be used in different methods
Paddle pad;
Ball ball(POSITION_TO_CENTRE_BALL,BEGINNING_OF_FALLING_BALL);
Brick bricks_array[70];
Brick b_last(218,BRICK_LINE_HEIGHT_BEGINNING-60);

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

void playNote(float analog_data[], float frequency, float duration, float volume){
	
	
}
void GameInitialization(){
	int8_t difficulty = -1;
	GLCD_Clear(Black);
	/////////////////////////////NEW button1_click()//////////////////////////////////
	while(difficulty==-1){				//difficulty not selected. As soon as it is selected, the game starts
		if(button1_click())					//BUTTON1 for easy
			difficulty=1;
		else if(button2_click())		//BUTTON2 for medium
			difficulty=2;
		else if(button3_click())		//BUTTON3 for hard
			difficulty=3;
		else;
	}
	
	pad.set_difficulty(difficulty);
	pad.draw();
	b_last.draw();
	int i=2;
	int j=0;
	int s=0;
	while(i*j<=END_OF_LINES){
		bricks_array[s].set(i,BRICK_LINE_HEIGHT_BEGINNING-10*j); 
		bricks_array[s].draw();
		i+=24;
		s++;
		if(i==BRICK_LINE_LENGTH_END){
			i=2;
			j++;
		}
	}
	
	while(ball.y>(PADDLE_WIDTH+PADDLE_Y)){
		ball.old_y = ball.y+1;
		ball.old_x = ball.x;
		ball.draw();
		ball.y--;
		delay(1);
	}
	/////////////////////////////////////////////////////////////////////
	/*while(1){																													 //
		ball.move(pad);																									 //	
		ball.draw();																										 //	
		bool hitttt = ball.check_collision_new(bricks_array[65]);				 //	 LOOK AT THIS
		if(hitttt)																											 //
			bricks_array[65].hit = true;																	 //
		delay(1);																												 //
	}			*/																														 //
	/////////////////////////////////////////////////////////////////////
}

void game(void const *argument){										
	for(;;){
		//osMutexWait(game_mutex_id,0);
		ball.move(pad);
		int i;
		for(i=0; i<70; i++){
			bool checked = ball.check_collision_new(bricks_array[i]);
			if(checked)
				bricks_array[i].hit = true;
		}
		bool c = ball.check_collision_new(b_last);
		if(c)
			b_last.hit = true;
		uint32_t direction_left = Button_GetState(0);
		uint32_t direction_right = Button_GetState(1);
		pad.move(direction_left,direction_right);
		//osMutexRelease(game_mutex_id);
		osDelay(GAME_DELAY);
	}
}

void refresh_screen(void const *argument){					
	for(;;){
		//osMutexWait(game_mutex_id,0);
		ball.draw();
		pad.draw();
		b_last.draw();
		//int i;
		//for(i=0; i<70; i++)
			//bricks_array[i]->draw();
		//osMutexRelease(game_mutex_id);
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

	//mutex
	//game_mutex_id = osMutexCreate(osMutex(game_mutex));
	
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
}
