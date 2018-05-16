#include "GLCD.h"
#include "Board_Buttons.h"
#include <stdio.h>
#include "game_elements.cpp"
#include "cmsis_os.h"
#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "DAC_LPC1768.h"

#define BRICK_LINE_HEIGHT_BEGINNING 270		//it leaves 50 pixel from the higher border of the screen to the first line of bricks
#define END_OF_LINES 1302				//add 186 to increase the #rows by 1, subtract 186 to decrease the #rows by 1.......CURRENTLY 7 ROWS
#define BRICK_LINE_LENGTH_END	241					
#define LAST_BRICK	217				//starting pixel of the last brick
#define POSITION_TO_CENTRE_BALL 117
#define BEGINNING_OF_FALLING_BALL 100

#define SCREEN_DELAY 15					//15 clock ticks--> 15ms
#define GAME_DELAY   5					//5 clock ticks-->5ms
								
#define SOUND_DELAY	1000				//40us -> 25Khz sound

//RTOS threads
osThreadId tid_screen;  				//thread for the refresh of the screen
osThreadId tid_game;					//thread for the game execution

//RTOS mutex
osMutexDef (game_mutex); // Declare mutex
osMutexId game_mutex_id; // Mutex ID

//global objects/variables to be used in different methods
Paddle pad;
Ball ball;
Brick bricks_array[70];
uint16_t total_hits; 	//shows total number of bricks hit

/*************************Methods used to check the click of a button*************************/
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

inline bool allButtons_click(){
	if (Button_GetState(0) == 1 && Button_GetState(1) == 1 && Button_GetState(2) == 1)
		return true;
	else
		return false;
}


/*************************Method to reset the whole system...regiser taken from the lpc datasheet*************************/
void system_reset(){
	SCB->AIRCR = (0x5FA<<SCB_AIRCR_VECTKEY_Pos)|SCB_AIRCR_SYSRESETREQ_Msk;		//writes 0x5FA in VECTKEY field [31:16] and sets SYSRESETREQ to 1
	for(;;);																																	//waits until the system has been reset
}

/*************************Initialization of the game*************************/
void GameInitialization(){
	total_hits = 0;
	int8_t difficulty = -1;
	GLCD_Clear(Black);				//LCD black while waiting for the difficulty
	if (pad.self) 					//debug mode
		difficulty = 4;
	while(difficulty == -1){			//difficulty not selected. As soon as it is selected, the game starts
		delay(20);
		if(button1_click())			//BUTTON1 for easy
			difficulty = 1;
		else if(button2_click())		//BUTTON2 for medium
			difficulty = 2;
		else if(button3_click())		//BUTTON3 for hard
			difficulty = 3;
		else if (allButtons_click())		//all buttons for automatic mode
			difficulty = 4;
		
	}
	pad.init(difficulty);								//set the size of the paddle
	pad.draw();										//draw the paddle
	ball.init(POSITION_TO_CENTRE_BALL,BEGINNING_OF_FALLING_BALL);	//set the position of the ball
	
	int64_t i=1;						//to leave 1 pixel from the border of the screen				
	int32_t j=0;						//brick line index
	int32_t s=0;						//array index
	while(i*j <= END_OF_LINES){			//loop for the creation and the representation of the 70 bricks
		bricks_array[s].set(i,BRICK_LINE_HEIGHT_BEGINNING-10*j); 
		bricks_array[s].draw();
		i += 24;													
		s++;
		if (i >= BRICK_LINE_LENGTH_END){
			i = 1;			
			j++;
		}
	}
	
	play_music(START,START_DURATION);	//music reproduction before the ball falls
	
	while(ball.y>(PADDLE_WIDTH+PADDLE_Y)){			//falling of the ball
		ball.old_y = ball.y+1;					//updates the old stored value of the ball
		ball.old_x = ball.x;					//updates the new stored value of the ball
		ball.draw();															
		ball.y--;							//decrements the y of the ball in order to create the falling
		delay(1);
	}
}

/*************************Method for the execution of the game. It's attached to the tid_game thread*************************/
void game(void const *argument){										
	for(;;){
		osMutexWait(game_mutex_id,0);
		ball.move(pad);
		int i;
		int loop_hits = 0;
		for(i=0; i<70 && loop_hits<3; i++){						//every execution of the method, it checks if the ball touches
			bool checked = ball.check_collision(bricks_array[i]);		//1 of the 70 bricks; if yes, hit is set to true and will be used
			if(checked){								//to change its color (i.e. to break it)
				bricks_array[i].hit = true;
				loop_hits++;
				total_hits++;
			}
		}
		uint32_t direction_left = Button_GetState(0);				//checks if the paddle goes left
		uint32_t direction_right = Button_GetState(1);				//checks if the paddle goes right
		pad.move(direction_left,direction_right, ball.x);			//to make the paddle move in the directon wanted
		
		//check win or loss
		if (ball.y <= 0){ 						//bottom of the screen...YOU LOST
			play_music(LOSS,LOSS_DURATION);			//sad music
			system_reset();						//reset of the system writing in the appropriate register
		}
		else if(total_hits == 70){ 					//no more bricks...YOU WON
			play_music(WIN,WIN_DURATION);				//happy music
			if (pad.self)						//if auto mode		
				GameInitialization();				//restart game
			else
				system_reset();					//reset of the system writing in the appropriate register
		}
		osMutexRelease(game_mutex_id);
		osDelay(GAME_DELAY);																							
	}
}

/*************************Method for the representation on screen of the elements of the game. It's attached to the tid_screen thread*************************/
void refresh_screen(void const *argument){					
	for(;;){
		osMutexWait(game_mutex_id,0);
		ball.draw();																											//displays the ball
		pad.draw();																												//displays the paddle
		osMutexRelease(game_mutex_id);
		osDelay(SCREEN_DELAY);
	}
}

/*************************Creation of the 2 threads*************************/
osThreadDef(game, osPriorityNormal, 1, 0);														
osThreadDef(refresh_screen, osPriorityNormal, 1, 0);

int main(void){
	if (osKernelInitialize () != osOK){  	//initialize the RTOS
								//exit with an error message
		return -1;
	}
	
	DAC_Initialize();				//initialization of the DAC, used to send data to the buzzer, which is directly linked to it
	GLCD_Init();				//initialization of the LCD
	Buttons_Initialize();			//initialization of the Buttons
	GameInitialization();			//initialization of the Game
	
	//mutex
	game_mutex_id = osMutexCreate(osMutex(game_mutex));
	
	//threads
	tid_game = osThreadCreate(osThread(game), NULL);
	if (tid_game == NULL) {
							//Failed to create the thread
	}
		
	tid_screen = osThreadCreate(osThread(refresh_screen), NULL);
	if (tid_screen == NULL) {  
							//Failed to create the thread
	}	
	
	if (osKernelStart() != osOK){ 	//start the kernel
							// kernel could not be started
		return -1;
	}
	
	osDelay(osWaitForever); 		//main thread waits forever
	
	//NO MAN'S LAND
	while(1);
}
