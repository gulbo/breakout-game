#include "GLCD.h"
#include "Board_Buttons.h"
#include "Board_Led.h"
#include <stdio.h>
void await(unsigned int j){
	int count;
	for(count=0; count<j; count++);
}

int main(void){
	
	GLCD_Init();
	LED_Initialize();
	GLCD_Clear(Black);
	//to be put into the rowinitiaization() function
	unsigned int i=2;
	unsigned int j=0;
	short brick_color;
	while(i*j<1210){			//1452 to have 8 rows, 1210 to have 7 rows ecc...
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
	/////////////////////////////////////////////////////////////////
	while(1){
	}
	return 0;
}
