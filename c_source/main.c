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
	GLCD_GameInitialization(1);
	while(1){
	}
	return 0;
}
