#include "GLCD.h"
#include "Board_Buttons.h"
#include "Board_Led.h"
#include <stdio.h>

void await(unsigned int j){
	int count;
	for(count=0; count<j; count++);
}

void GameInitialization(unsigned int difficulty){
	//TODO IMPLEMENT WITH OBJECTS
	GLCD_Clear(Black);
	int i=2;
	int j=0;
	short brick_color;
	while(i*j<=1210){			//1452 to have 8 rows, 1210 to have 7 rows ecc...
		switch(j){
			case 0:
				brick_color=Purple;
				break;
			case 1:
				brick_color=Blue;
				break;
			case 2:
				brick_color=Cyan;
				break;
			case 3:
				brick_color=DarkGreen;
				break;
			case 4:
				brick_color=Green;
				break;
			case 5:
				brick_color=Yellow;
				break;
			case 6:
				brick_color=Red;
				break;
		}
		GLCD_DrawRect(270-10*j,i,8,20,brick_color);
		i+=24;
		if(i==242){
			i=2;
			j++;
		}
	}
	GLCD_DrawRect(270-10*j,218,8,20,brick_color);
	
	switch(difficulty){
		case 0:
			GLCD_DrawRect(5,90,10,60,White);
			break;		
		case 1:
			GLCD_DrawRect(5,100,10,40,White);
			break;
		case 2:
			GLCD_DrawRect(5,110,10,20,White);
			break;
	}
	int k=100;
	while(k>=15){
		GLCD_DrawRect(k+1,117,6,6,Black);
		GLCD_DrawRect(k,117,6,6,Magenta);
		k--;
		delay(1);
	}
}

int main(void){
	
	GLCD_Init();
	LED_Initialize();
	GameInitialization(1);
	while(1){
	}
	return 0;
}
