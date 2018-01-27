/*-----------------------------------------------------------------------------
 * Name:    DAC_MCB1700.c
 * Purpose: D/A Converter interface for MCB1700 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2016 ARM LIMITED

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
#include "DAC_LPC1768.h"

#define DAC_RESOLUTION        10        /* Number of D/A converter bits       */

/* DAC pins:
   - AOUT: P0_26
*/

const PIN DAC_PIN[] = {
  {0, 26},
};

/**
  \fn          int32_t DAC_Initialize (void)
  \brief       Initialize Digital-to-Analog Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t DAC_Initialize (void) {

  LPC_SC->PCONP |= (1 << 15);           /* Enable power to IOCON              */

  PIN_Configure (DAC_PIN[0].Portnum, DAC_PIN[0].Pinnum, PIN_FUNC_2, 0, 0);
  
  LPC_DAC->DACR  =  0;                  /* Initialize control register        */

  return 0;
}

/**
  \fn          int32_t DAC_Uninitialize (void)
  \brief       De-initialize Digital-to-Analog Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t DAC_Uninitialize (void) {

  PIN_Configure (DAC_PIN[0].Portnum, DAC_PIN[0].Pinnum, 0, 0, 0);

  return 0;
}

/**
  \fn          int32_t DAC_SetValue (uint32_t val)
  \brief       Set value to be converted
  \param[in]   val  value to convert
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t DAC_SetValue (uint32_t val) {

  LPC_DAC->DACR = (LPC_DAC->DACR & (~(0x3FF << 6))) | (val & (0x3FF << 6));

  return 0;
}

/**
  \fn          uint32_t DAC_GetResolution (void)
  \brief       Get resolution of Digital-to-Analog Converter
  \returns     Resolution (in bits)
*/
uint32_t DAC_GetResolution (void) {
  return DAC_RESOLUTION;
}
