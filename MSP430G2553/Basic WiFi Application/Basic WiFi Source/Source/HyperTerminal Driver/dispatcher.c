/*****************************************************************************
*
*  dispatcher.c  - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

//*****************************************************************************
//
//! \addtogroup dispatcher_api
//! @{
//
//*****************************************************************************

#include <msp430.h>
//#include <msp430g2553.h>
#include "dispatcher.h"
#include "board.h"




///////////////////////////////////////////////////////////////////////////////////////////////////////////                     
//__no_init is used to prevent varible's initialize.                                                    ///
//for every IDE, exist different syntax:          1.   __CCS__ for CCS v5                               ///
//                                                2.  __IAR_SYSTEMS_ICC__ for IAR Embedded Workbench    ///
// *CCS does not initialize variables - therefore, __no_init is not needed.                             ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __CCS__

unsigned char g_ucUARTBuffer[UART_IF_BUFFER];
unsigned char g_ucLength;

#elif __IAR_SYSTEMS_ICC__

__no_init unsigned char g_ucUARTBuffer[UART_IF_BUFFER];
__no_init unsigned char g_ucLength;

#endif

int uart_have_cmd = 0;



//*****************************************************************************
//
//! Configure  the UART
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Configure  the UART
//
//*****************************************************************************

void
DispatcherUARTConfigure(void)
{
    // Configure UART pins P1.1 & P1.2
    UART1_PORT |= UART1_PIN + UART2_PIN;
    UART2_PORT  |= UART1_PIN + UART2_PIN;
    
    UCA0CTL1 |= UCSWRST; 	                 
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    
    // configure clock scaler : UCA0BR1*256 + UCA0BR0 * 9600
    UCA0BR0 = 130;                            // 12MHz 9600
    UCA0BR1 = 6;  
    UCA0MCTL = 0;                         // Modulation UCBRSx = 1
    
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
     
    g_ucLength = 0;
    
    memset(g_ucUARTBuffer,0,sizeof(g_ucUARTBuffer));
  
}



//*****************************************************************************
//
//! UARTIntHandler
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  Handles RX and TX interrupts
//
//*****************************************************************************
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  unsigned char last = UART_RX_BUF_OVERFLOW;
    
    last = UCA0RXBUF;
    
    if(g_ucLength < UART_IF_BUFFER)
    {
        g_ucUARTBuffer[g_ucLength] = last;    
    }
    else /* UART RX BUffer max size is exceeded , trigger illegal command */
    {
        memset(g_ucUARTBuffer,0,sizeof(g_ucUARTBuffer));
        g_ucUARTBuffer[1] = UART_RX_BUF_OVERFLOW;  
    }    
    
    
    if(last == CARRIAGE_RETURN)
    {  
        g_ucLength                 = 0;
        uart_have_cmd              = 1;
      
        __bic_SR_register_on_exit(LPM3_bits);
        __no_operation();
    }
    else
    {
        g_ucLength++;
    }
    
}


//*****************************************************************************
//
//! DispatcherUartSendPacket
//!
//!  \param  inBuff    pointer to the UART input buffer
//!  \param  usLength  buffer length
//!
//!  \return none
//!
//!  \brief  The function sends to UART a buffer of given length 
//
//*****************************************************************************
void
DispatcherUartSendPacket(unsigned char *inBuff, unsigned short usLength)
{
	while (usLength)
	{
		while (!(IFG2&UCA0TXIFG));
		UCA0TXBUF = *inBuff;
		usLength--;
		inBuff++;
	}
}


//*****************************************************************************
//
//!  \brief Resets the UART buffer and pointer
//!
//!  \param  none
//!
//!  \return none
//
//*****************************************************************************
void resetUARTBuffer()
{
    g_ucLength = 0;
    memset(g_ucUARTBuffer,0,sizeof(g_ucUARTBuffer));    
}




//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
