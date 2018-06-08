#include "GLCD.h"
#include "Board_Buttons.h"
#include <stdio.h>
#include "game_elements.cpp"
#include "cmsis_os.h"
#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "DAC_LPC1768.h"

//#define DEBUG

#define BRICK_LINE_HEIGHT_BEGINNING 270		//it leaves 50 pixel from the higher border of the screen to the first line of bricks
#define END_OF_LINES 1302				//add 186 to increase the #rows by 1, subtract 186 to decrease the #rows by 1.......CURRENTLY 7 ROWS
#define BRICK_LINE_LENGTH_END	241					
#define LAST_BRICK	217				//starting pixel of the last brick
#define POSITION_TO_CENTRE_BALL 117
#define BEGINNING_OF_FALLING_BALL 100

#define SCREEN_DELAY 15					//15 clock ticks--> 15ms
#define GAME_DELAY   5					//5 clock ticks-->5ms
								
#define SOUND_DELAY	1000				//40us -> 25Khz sound

#define EASY 1						//difficulty
#define MEDIUM 2
#define HARD 3
#define AUTO 4

//RTOS threads
osThreadId tid_screen;  				//thread for the refresh of the screen
osThreadId tid_game;					//thread for the game execution

//RTOS mutex
osMutexDef (game_mutex); 				// Declare mutex
osMutexId game_mutex_id; 				// Mutex ID

//global objects/variables to be used in different methods
Paddle pad;
Ball ball;
Brick bricks_array[70];

int8_t difficulty;
uint16_t total_hits; 					//shows total number of bricks hit
uint64_t game_points;					//points of the game

/*************************Methods used to check the click of a button*************************/
inline bool button1_click(){
	static uint32_t state = 0;
	uint32_t  new_state = Button_GetState(0);
	if (state != new_state){
		if (new_state == 1){ //button clicked
			state = new_state;
			return true;
		}
		else{  		//unclick of the button
			state = new_state;
			return false;
		}	
	}
	return false; //in case of no change of state
}

inline bool button2_click(){
	static uint32_t state = 0;
	uint32_t  new_state = Button_GetState(1);
	if (state != new_state){
		if (new_state == 1){ //button clicked
			state = new_state;
			return true;
		}
		else{  		//unclick of the button
			state = new_state;
			return false;
		}	
	}
	return false; //in case of no change of state
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

inline unsigned int strlen(char* s){
	unsigned int i = 0;
	while(*s){
		i++;
		s++;
	}
	return i;
}

inline void display_points(){
	char str_points[12];
	GLCD_SetTextColor(Red);			
	GLCD_SetBackColor(Black);
	sprintf(str_points, "%lu",(unsigned long) game_points / (difficulty*2)); // /6 points for HARD, /4 for MEDIUM, /2 for easy
	GLCD_DisplayString(0,SCREEN_HEIGHT-CHAR1_WIDTH*strlen(str_points),1,(unsigned char*)str_points,PIXELS);
}
struct Button{
	const char* name;
	unsigned int x;
	unsigned int y;
	unsigned int text_color;
	unsigned int button_color;
		
	//constructor
	Button(const char* s, const int p_x, const int p_y) : x(p_x), y(p_y){
		name = s;
		if (p_x == -1)    //automatically centre
			centre_h();
		select(false);
	}
	
	void set_colors(unsigned int but, unsigned int tex){
		text_color = tex;
		button_color = but;
	}
	
	void select(bool x){
		if(x){
			text_color = Blue;
			button_color = Yellow;
		}
		else{
			text_color = Black;
			button_color = (Yellow+White)/2;
		}
	}
	void draw(){
		GLCD_SetTextColor(text_color);			
		GLCD_SetBackColor(button_color);
		GLCD_DisplayString(SCREEN_WIDTH-y-CHAR1_HEIGHT,x,1,(unsigned char*)name,PIXELS);
	}
	
	void centre_h(){
		unsigned int button_width = strlen((char*)name) * CHAR1_WIDTH; //num of pixels occupied
		x = (SCREEN_HEIGHT-button_width) / 2;
	}
	
};

/*************************Method to reset the whole system...regiser taken from the lpc datasheet*************************/
void system_reset(){
	SCB->AIRCR = (0x5FA<<SCB_AIRCR_VECTKEY_Pos)|SCB_AIRCR_SYSRESETREQ_Msk;		//writes 0x5FA in VECTKEY field [31:16] and sets SYSRESETREQ to 1
}

void Explosion(uint16_t y, uint16_t x, uint16_t dim){				
	for(int d=1; d<=3; d++){
		GLCD_DrawRect(x-dim*d,y-dim*d,dim,dim,White);
		GLCD_DrawRect(x-dim*d,y+dim*(d-1),dim,dim,White);
		GLCD_DrawRect(x+dim*(d-1),y-dim*d,dim,dim,White);
		GLCD_DrawRect(x+dim*(d-1),y+dim*(d-1),dim,dim,White);
		delay(3);
		GLCD_DrawRect(x-dim*d,y-dim*d,dim,dim,Black);
		GLCD_DrawRect(x-dim*d,y+dim*(d-1),dim,dim,Black);
		GLCD_DrawRect(x+dim*(d-1),y-dim*d,dim,dim,Black);
		GLCD_DrawRect(x+dim*(d-1),y+dim*(d-1),dim,dim,Black);
	}
}

void introduction(){
	uint16_t i = 228;
	uint16_t j = 170;
	GLCD_DrawRect((SCREEN_WIDTH-20)/3,(SCREEN_HEIGHT-60)/2, 20, 60, Yellow);
	while(j>=62){
		GLCD_DrawRect(i,j,8,8,Red);
		delay(2);
		GLCD_DrawRect(i,j,8,8,Black);
		j--;
		if(j>=116){
			i=i-2;
		}else{
			i=i+2;
		}
	}
	GLCD_Clear(Black);
	Explosion(j,i,3);
	GLCD_DisplayString(SCREEN_WIDTH-200,(SCREEN_HEIGHT-13*CHAR1_WIDTH)/2,1,(unsigned char*)"BREAKOUT GAME",PIXELS);
	GLCD_DisplayString(SCREEN_WIDTH-100,(SCREEN_HEIGHT-12*CHAR0_HEIGHT)/2,0,(unsigned char*)"Developed by",PIXELS);
	GLCD_DisplayString(SCREEN_WIDTH-CHAR0_HEIGHT,0,0,(unsigned char*)"Andrea Gulberti",PIXELS);
	GLCD_DisplayString(SCREEN_WIDTH-CHAR0_HEIGHT,SCREEN_HEIGHT-11*CHAR0_HEIGHT,0,(unsigned char*)"Simone Fini",PIXELS);
	delay(500);
	GLCD_Clear(Black);
}

/*************************Initialization of the game*************************/
 void GameInitialization(){
	game_points = 0;
	total_hits = 0;
	difficulty = -1;
	GLCD_Clear(Black);				//LCD black while waiting for the difficulty
	#ifdef DEBUG					//debug mode
	difficulty = AUTO;
	#endif
	if (pad.self) 					//if not a cold start, but playing in auto-mode, don't ask the diffic
		difficulty = AUTO;
	introduction();
	Button* arr[3];
	Button b0("    EASY    ",-1,180);		
	Button b1("   MEDIUM   ",-1,140);
	Button b2("    HARD    ",-1,100);
	
	arr[0]=&b0; arr[1]=&b1; arr[2]=&b2;		//save the buttons in an array
	int selected = 1;					//medium default selected
	arr[selected]->select(true);
	b0.draw(); b1.draw(); b2.draw();		//draw all the buttons
	while(difficulty == -1){			//difficulty not selected. As soon as it is selected, the game starts
		delay(10);
		if(button1_click()){			//UP
			arr[selected]->select(false);
			arr[selected]->draw();
			selected--;
		}
		else if (button2_click()){		//DOWN
			arr[selected]->select(false);
			arr[selected]->draw();
			selected++;
		}
		else if (button3_click()){		//CLICK
			difficulty = selected+1;	//difficulty from 1 to 3
		}
		else if (allButtons_click()){		//all buttons for automatic mode
			difficulty = AUTO;
		}
		if (selected < 0)
			selected = 0;
		else if (selected > 2)
			selected = 2;
		arr[selected]->select(true);
		arr[selected]->draw();
	}
	
	GLCD_Clear(Black);
	pad.init(difficulty);								//set the size of the paddle
	pad.draw();										//LEAVE IT HERE
	ball.init(POSITION_TO_CENTRE_BALL,BEGINNING_OF_FALLING_BALL);	//set the position of the ball
	
	int64_t i=1;						//to leave 1 pixel from the border of the screen				
	int32_t j=0;						//brick line index
	int32_t s=0;						//array index
	while(i*j <= END_OF_LINES){				//loop for the creation and the representation of the 70 bricks
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
		for(i=0; i<70 && loop_hits<3; i++){					//every execution of the method, it checks if the ball touches
			if(ball.check_collision(bricks_array[i])){		//one of the 70 bricks
				loop_hits++;
				total_hits++;
			}
		}

		uint32_t dir_left = Button_GetState(0);			//checks if the paddle goes left
		uint32_t dir_right = Button_GetState(1);			//checks if the paddle goes right
		pad.move(dir_left,dir_right, ball.x);			//move it	
		
		//check win or loss
		if (ball.y <= 0){ 						//bottom of the screen...YOU LOSE
			GLCD_SetTextColor(Yellow);			
			GLCD_SetBackColor(Black);
			const char* str = "YOU LOSE!";
			GLCD_DisplayString(120,50,1,(unsigned char*)str,PIXELS);
			play_music(LOSS,LOSS_DURATION);			//sad music
			system_reset();						//reset of the system writing in the appropriate register
		}
		else if(total_hits == 70){ 					//no more bricks...YOU WIN
			GLCD_SetTextColor(Blue);			
			GLCD_SetBackColor(Black);
			const char* str = "YOU WIN!";
			GLCD_DisplayString(120,60,1,(unsigned char*)str,PIXELS);
			play_music(WIN,WIN_DURATION);				//happy music	
			if (pad.self)						//if auto mode		
				GameInitialization();				//restart game
			else
				system_reset();					//reset of the system writing in the appropriate register
		}
		
		game_points++;							//increase points of the game
		osMutexRelease(game_mutex_id);
		osDelay(GAME_DELAY);																							
	}
}

/******Method for the representation on screen of the elements of the game.
*******It's attached to the tid_screen thread*****************************/
void refresh_screen(void const *argument){					
	for(;;){
		osMutexWait(game_mutex_id,0);
		display_points();					//display points
		if(total_hits<70)
		ball.draw();					//displays the ball
		pad.draw();						//displays the paddle
		int i;
		for(i=0; i<70; i++)	 			//displays the bricks (if modified)
			bricks_array[i].draw();			
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
