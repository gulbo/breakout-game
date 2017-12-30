/*-----------------------------------------------------------------------------
 * Name:    Buttons_MCB1700.c
 * Purpose: Buttons interface for MCB1700 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "Board_Buttons.h"

#define BUTTONS_COUNT                  (3)

/* Button pins:
		- BUTTON1/KEY1: P2.11 = GPIO2[11]
		- BUTTON2/KEY2: P2.12 = GPIO2[12]
		- BUTTON3/INT0: P2.10 = GPIO2[10]
*/


/* Button pin definitions */
const PIN BUTTON_PIN[] = {
  {2, 11},
	{2, 12},
	{2, 10}
};


/**
  \fn          int32_t Buttons_Initialize (void)
  \brief       Initialize buttons
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Buttons_Initialize (void) {

  /* Enable GPIO clock */
  GPIO_PortClock   (1);

  /* Configure Pins */
  PIN_Configure (BUTTON_PIN[0].Portnum, BUTTON_PIN[0].Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
  GPIO_SetDir   (BUTTON_PIN[0].Portnum, BUTTON_PIN[0].Pinnum, GPIO_DIR_INPUT);
	
	PIN_Configure (BUTTON_PIN[1].Portnum, BUTTON_PIN[1].Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
  GPIO_SetDir   (BUTTON_PIN[1].Portnum, BUTTON_PIN[1].Pinnum, GPIO_DIR_INPUT);
	
	PIN_Configure (BUTTON_PIN[2].Portnum, BUTTON_PIN[2].Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
  GPIO_SetDir   (BUTTON_PIN[2].Portnum, BUTTON_PIN[2].Pinnum, GPIO_DIR_INPUT);
  
  return 0;
}

/**
  \fn          int32_t Buttons_Uninitialize (void)
  \brief       De-initialize buttons
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Buttons_Uninitialize (void) {

  /* Unconfigure Pins */
  PIN_Configure (BUTTON_PIN[0].Portnum, BUTTON_PIN[0].Pinnum, 0, 0, 0);
	PIN_Configure (BUTTON_PIN[1].Portnum, BUTTON_PIN[0].Pinnum, 0, 0, 0);
	PIN_Configure (BUTTON_PIN[2].Portnum, BUTTON_PIN[0].Pinnum, 0, 0, 0);

  return 0;
}

/**
  \fn          uint32_t Buttons_GetState (void)
  \brief       Get buttons state
  \returns     Buttons state
*/
uint32_t Button_GetState (int n) {		//n must be included between 0 and 2, indicating button1, button2, button3
  
	uint32_t val;
  val = 0;
  if (!(GPIO_PinRead (BUTTON_PIN[n].Portnum, BUTTON_PIN[n].Pinnum))) 
		val = 1;

  return val;
}

/**
  \fn          uint32_t Buttons_GetCount (void)
  \brief       Get number of available buttons
  \return      Number of available buttons
*/
uint32_t Buttons_GetCount (void) {
  return BUTTONS_COUNT;
}
