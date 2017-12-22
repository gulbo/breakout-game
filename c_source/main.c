#include "GLCD.h"
#include "Board_Buttons.h"
#include "Board_Led.h"
#include <stdio.h>

int main(void){
	
	GLCD_Init();
	LED_Initialize();
	
	while(1){
		GLCD_Clear(Blue);
		int i;
		for(i=0; i<100000000; i+=6);
		GLCD_Clear(Red);
		for(i=0; i<100000000; i+=6);
	}
	return 0;
}
