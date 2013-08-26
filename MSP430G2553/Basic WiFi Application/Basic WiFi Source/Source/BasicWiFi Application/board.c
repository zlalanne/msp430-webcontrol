/*****************************************************************************
*
*  board.c - FRAM board functions
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

#include "msp430g2553.h"
#include "wlan.h" 
#include "evnt_handler.h"    
#include "nvmem.h"
#include "socket.h"
#include "netapp.h"
#include "board.h"




#define SMART_CONFIG_SET        0xAA
#define SMART_CONFIG_CLEAR      0xFF

 
extern unsigned char * ptrFtcAtStartup;
//*****************************************************************************
//
//! pio_init
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Initialize the board's I/O
//
//*****************************************************************************    

void pio_init()
{
  // Init the device with 16MHz DCOCLCK.
  initClk();
  
  ConfigureSwitch();  
  
  // P1.0 - WLAN enable full DS
  WLAN_EN_PORT_OUT &= ~WLAN_EN_PIN;
  WLAN_EN_PORT_DIR |= WLAN_EN_PIN;
  WLAN_EN_PORT_SEL2 &= ~WLAN_EN_PIN; 
  WLAN_EN_PORT_SEL &= ~WLAN_EN_PIN;
  
  
  // Configure SPI IRQ line on P2.6
  SPI_IRQ_PORT_DIR  &= (~SPI_IRQ_PIN);	
  SPI_IRQ_PORT_SEL2 &= ~SPI_IRQ_PIN; 
  SPI_IRQ_PORT_SEL &= ~SPI_IRQ_PIN;
  
  
  // Configure the SPI CS to be on P2.7
  SPI_CS_PORT_OUT |= SPI_CS_PIN;
  SPI_CS_PORT_DIR |= SPI_CS_PIN;
  SPI_CS_PORT_SEL2 &= ~SPI_CS_PIN; 
  SPI_CS_PORT_SEL &= ~SPI_CS_PIN;
  
  
  __delay_cycles(12000000);
  
  
  
  
  initLEDs();
}
//*****************************************************************************
//
//! ConfigureSwitch
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Configure FRAM switches
//
//*****************************************************************************

void ConfigureSwitch(void)
{
    // Enable switches 
    // P1.3 and p3.3 configured as switches
  
    P1OUT |= BIT3;                      // Configure pullup resistor  
    P1DIR &= ~(BIT3);                  // Direction = input
    P1REN |= BIT3;                     // Enable pullup resistor
    P1IES |= (BIT3);                   // P1.3 Hi/Lo edge interrupt  
    P1IFG = 0;                         // P1 IFG cleared
    P1IE |= BIT3;                       // P1.3 interrupt enabled
    
    // Enable First Time Config Prefix changing jumper
    // To detect if pulled high by VCC
    
    // P3.3 is our High signalx
    P3DIR |= BIT3;
    P3OUT |= BIT3;
        
    // P2.2 Configure pulled low and will detect a hi/low transition
    
    P2DIR &= ~(BIT0);             // P2.0 As Input
    P2OUT &= ~(BIT0);             // P2.0 With Pulldown
    P2REN |= BIT0;                // P2.0 Enable Pulldown
    P2IES &= ~(BIT0);             // P2.0 Lo/Hi edge interrupt
    P2IFG &= ~(BIT0);             // P2.0 IFG cleared
    P2IE |= BIT0;                 // P2.0 interrupt enabled
    

}
//*****************************************************************************
//
//! ReadWlanInterruptPin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  return wlan interrup pin
//
//*****************************************************************************

long ReadWlanInterruptPin(void)
{
	return (P2IN & BIT6);
}

//*****************************************************************************
//
//! Enable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
//
//*****************************************************************************


void WlanInterruptEnable()
{
	__bis_SR_register(GIE);
	P2IES |= BIT6;
	P2IE |= BIT6;
}

//*****************************************************************************
//
//! Disable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
//
//*****************************************************************************


void WlanInterruptDisable()
{
	P2IE &= ~BIT6;
}


//*****************************************************************************
//
//! WriteWlanPin
//!
//! \param  new val
//!
//! \return none
//!
//! \brief  void
//
//*****************************************************************************

void WriteWlanPin( unsigned char val )
{
	if (val)
	{
		P1OUT |= BIT0;	
	}
	else
	{
		P1OUT &= ~BIT0;
	}
}
//*****************************************************************************
//
//! wakeup_timer_init (Timer1_A3 , (CCR0) Compare/Capture = 0)
//! ISR : TIMER1_A0_VECTOR 
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  The function initializes the timer 
//
//*****************************************************************************
void wakeup_timer_init(void)
{
        
    // disable interrupt    
    TA1CCTL0 &= ~CCIE; 
        
    // stop timer    
    TA1CTL |= MC_0;

    /**************************************************************************/
    // run the timer from ACLCK, and enable interrupt of Timer A
    //  ACLK clock is sourced by VLO = 12kHz  
    //  TASSELx = TASSEL_1 = ACLK 
    //  MCx     = MC_1 = 01 - Up mode: the timer counts up to TACCR0
    //  TACLR   = 0x4 - Timer_A clear. Setting this bit resets TAR, 
    //                  the clock divider, and the count direction
    //  ID_3    = (3*0x40u) - Timer A input divider: 3 -  div 8 
    /**************************************************************************/
    TA1CTL |= TACLR;     
    TA1CTL |= (TASSEL_1 + MC_1 + ID_3);

        
    // Configure the timer to wakeup every 500 millisecond 
    //  12KHz div 8 div 8 = ~ 187.5 Hz
    TA1CCR0 = 0x5d ; // 93 
    
    // enabled timer interrupt , once the timer was configured  
    TA1CCTL0 |= CCIE;
}


//*****************************************************************************
//
//! wakeup_timer_disable
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  Stop the timer
//
//*****************************************************************************
void wakeup_timer_disable(void)
{
        // disable Timer1_A3 Capture/Control CCR0
        TA1CCTL0 &= ~CCIE;
        
       // stop timer 
	TA1CTL |= MC_0;         
}


// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{              
	__bic_SR_register_on_exit(LPM3_bits);
	__no_operation();                          // For debugger
}




//*****************************************************************************
//
//! init clk
//!
//!  \param  None
//!
//!  \return none
//!
//!  \Init the device with 16 MHz DCOCLCK.
//
//*****************************************************************************
void initClk(void)
{
	 
    //
    // Init the device with 16 MHz DCOCLCK.
    // SMCLCK which will source also SPI will be sourced also by DCO
    // 
    /*****************************************************/
    /* Name: BCSCTL1  hex value : 0xF9                   */
    /* DIVAx  = 3 , divider for ACLK = 8                 */
    /* XTS    = 1   High Frequency mode                  */
    /* XT2OFF = 1 , XT2 is off not used for MCLK & SMCLK */
    
    /*----------------------------------------------*/
    /* bits: 7    |  6   |  5   4  |  3  2  1  0    */
    /*----------------------------------------------*/
    /****| XT2OFF | XTS  |  DIVAx  |     RSELx      */
    /****|------------------------------------------*/
    /****|   1    |  1   |  1 |  1 | 1 | 0 | 0 | 1  */
    /****|------------------------------------------*/    
    BCSCTL1     =       CALBC1_16MHZ;
    
    
    /************************************************/
    /* Name: DCOCTL  hex value : 0xF8               */
    /*----------------------------------------------*/
    /* bits: 7    6   5    4    3    2    1    0    */
    /*----------------------------------------------*/
    /****|     DCOx      |         MODx           | */
    /****|------------------------------------------*/
    /****|   1 | 1  | 1  | 1  | 1  | 0  | 0  | 0  | */
    /****|------------------------------------------*/
    DCOCTL      =       CALDCO_16MHZ;
    
    /*******************************************************/
    /* ACLK (auxillary clock) clock is used for the Timers */
    /* ACLK = VLO = ~ 12 KHz                               */      
    /*******************************************************/
    BCSCTL3 = 0;
    BCSCTL3 |= LFXT1S_2;
    
#ifdef CC3000_TINY_DRIVER        
    /**************************************************************/
    /* Init FLASH controller clock                                */
    /* FSSEL2 -  = SMCLK = 16 MHz                                 */
    /* FWKEY  -  Flash key for write                              */
    /* Flash(flash timing generator) = 257-476 kHz                */
    /* MCLK div (FN5 + FN3 + FN1 + FN0) = 16 : 32 +8 + 2 + 1 + 1  */
    /*                      16 MHz : 44 = ~ 360 kHz               */
    /**************************************************************/
    FCTL2 = FWKEY + FSSEL_2 + (FN5 + FN3 + FN1 + FN0);
#endif    
    
    P1SEL |= BIT4; 
    P1DIR |= BIT4;
}

//*****************************************************************************
//
//! \brief  Starts timer that handles switch debouncing
//! Button S2 interrupt
//! ISR: PORT1_VECTOR
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void StartDebounceTimer()
{
  
  P1IFG &= ~BIT3;
  P1IES ^=  BIT3;
  
  //***********************************
  // programm timer : Timer0_A3, CCR1
  // INT Vector: TIMER0_A1_VECTOR
  //***********************************
  
  // disable Time1_A3 interrupt
  TA0CCTL1 &= (~CCIE); 
  
  // SMCLK, continuous mode
  TA0CTL = TASSEL_2 + MC_2;   

  // 16000 @ 16 MHz : ~ 1ms
  TA0CCR1 = 16000;
  
  
  // enable Time1_A3 interrupt
  TA0CCTL1 = CCIE; //TACCR0 interrupt enabled

}

//*****************************************************************************
//
//! \brief  Stops timer that handles switch debouncing
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void StopDebounceTimer()
{  
  //  Timer0_A3, CCR1
  TA0CCTL1 &= ~CCIE;                                                                                                                                                                                 
}

//*****************************************************************************
//
//! Initialize LEDs
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Initializes LED Ports and Pins
//
//*****************************************************************************
void initLEDs()
{
 
}
//*****************************************************************************
//
//! Turn LED On
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Turns a specific LED Off
//
//*****************************************************************************
void turnLedOn(char ledNum)
{

}
	
	
//*****************************************************************************
//
//! Turn LED Off
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Turns a specific LED Off
//
//*****************************************************************************    
void turnLedOff(char ledNum)
{                     
 
}

//*****************************************************************************
//
//! toggleLed
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Toggles a board LED
//
//*****************************************************************************    

void toggleLed(char ledNum)
{
   
}

//*****************************************************************************
//
//! \brief  check if Smart Config flag was set
//!
//! \param  none
//!
//! \return indication if flag is set
//!
//
//*****************************************************************************
long IsFTCflagSet()
{  
  unsigned char ret_value = 0;
  
  
  ret_value = ReadFlashByte( (unsigned char*)(&ptrFtcAtStartup));
  
  if(ret_value == SMART_CONFIG_SET)
    return(1);
  else
    return(0);
     
}

//*****************************************************************************
//
//! \brief  set Smart Config flag when S2 was pressed
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void SetFTCflag()
{ 
    WriteFlashByte( (unsigned char*)(&ptrFtcAtStartup),SMART_CONFIG_SET);
}

//*****************************************************************************
//
//! \brief  clear Smart Config flag when S2 was pressed
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void ClearFTCflag()
{ 
    WriteFlashByte( (unsigned char*)(&ptrFtcAtStartup),SMART_CONFIG_CLEAR);
}


//*****************************************************************************
//
//! \brief  Dissable S2 switch interrupt
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void DissableSwitch()
{  
  // disable switch interrupt
  P1IFG &= ~BIT3;                // Clear P1.3 IFG
  P1IE &= ~(BIT3);               // P1.3 interrupt disabled
  P1IFG &= ~BIT3;                // Clear P1.3 IFG
  
  P1IFG = 0;
}

//*****************************************************************************
//
//! \brief  Restore S2 switch interrupt
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void RestoreSwitch()
{              
    // Restore Switch Interrupt
    P1IFG &= ~BIT3;                 // Clear P1.3 IFG
    P1IE |= BIT3;                   // P1.3 interrupt enabled
    P1IFG &= ~BIT3;                 // Clear P1.3 IFG
}

 //*****************************************************************************
//
//! \brief  Indication if the switch is still pressed
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
long switchIsPressed()
{  
          
if(!(P1IN & BIT3))
  return 1;
 
return 0;

}

//*****************************************************************************
//
//! \brief  Restarts the MSP430
//!
//! Restarts the MSP430 completely. One must be careful
//!
//! \return never
//!
//
//*****************************************************************************    
void restartMSP430()
{
  
    WDTCTL |= 0x1A00u;
        
        
}


//*****************************************************************************
//
//! \brief  WriteFlashByte
//!
//! 
//!
//! \return never
//!
//
//*****************************************************************************    
void WriteFlashByte(unsigned char* addr, unsigned char value)
{
  unsigned char *Flash_ptr;               // Flash pointer
 
 __disable_interrupt();
    
  Flash_ptr = (unsigned char *) addr;             // Initialize Flash pointer
  FCTL3         = FWKEY;                          // Clear LOCK bit
  FCTL1         = FWKEY + ERASE;                  // Set Erase bit
  
  *Flash_ptr    = 0;
  
  FCTL1         = FWKEY;
  FCTL3         = FWKEY + LOCK;
  
  FCTL1         = FWKEY + WRT;                    // Set WRT bit for write operation
  FCTL3         = FWKEY;                          // Clear LOCK bit 
  
  *Flash_ptr    = value;                               
  
  FCTL1         = FWKEY;                          // Clear WRT bit
  FCTL3         = FWKEY + LOCK;                   // Set LOCK bit      

 __enable_interrupt();
  
}


//*****************************************************************************
//
//! \brief  WriteFlashByte
//!
//! 
//!
//! \return never
//!
//
//*****************************************************************************    
unsigned char ReadFlashByte(unsigned char* addr)
{
    unsigned char *Flash_ptr;                          // Flash pointer
    unsigned char ret_value = 0;

 __disable_interrupt();
    
    Flash_ptr = (unsigned char *) addr;
    FCTL1 = FWKEY + ERASE;
    FCTL3 = FWKEY;
    FCTL1 = FWKEY + WRT;
    
    ret_value = *Flash_ptr;
    
    FCTL1 = FWKEY; 
    FCTL3 = FWKEY + LOCK;

__enable_interrupt();
     
    return(ret_value);
}

//Catch interrupt vectors that are not initialized.

#ifdef __CCS__
#pragma vector=PORT1_VECTOR, WDT_VECTOR, USCIAB0TX_VECTOR, TIMER1_A1_VECTOR, TIMER0_A1_VECTOR, TIMER0_A0_VECTOR, ADC10_VECTOR, NMI_VECTOR, COMPARATORA_VECTOR
__interrupt void Trap_ISR(void)
{
  while(1);
}

#endif
