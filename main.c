#include "LPC11xx.h"                    // Device header
#define COM4_ON LPC_GPIO2->DATA &=~(1<<8)
#define COM3_ON LPC_GPIO2->DATA &=~(1<<9)
#define COM2_ON LPC_GPIO2->DATA &=~(1<<10)
#define COM1_ON LPC_GPIO2->DATA &=~(1<<11)
#define ALL_OFF LPC_GPIO2->DATA |=(1<<8)+(1<<9)+(1<<10)+(1<<11)
uint8_t table[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uint8_t DispBuffer[4];
uint8_t time[2];
uint32_t timeCounter;
uint8_t time_1s_ok;

void DisplayScan()
{
	static uint16_t com;
	com++;
	if(com>=4)
		com=0;
	ALL_OFF ;
	switch(com)
	{
		case 0:
			  LPC_GPIO2->DATA &=~0x0FF;
		    LPC_GPIO2->DATA |=DispBuffer [0];
			  COM1_ON ;
		    break ;
		case 1:
			  LPC_GPIO2->DATA &=~0x0FF;
		    LPC_GPIO2->DATA |=DispBuffer [1];
			  COM2_ON ;
		    break ;
		case 2:
			  LPC_GPIO2->DATA &=~0x0FF;
		    LPC_GPIO2->DATA |=DispBuffer [2];
			  COM3_ON ;
		    break ;
		case 3:
			  LPC_GPIO2->DATA &=~0x0FF;
		    LPC_GPIO2->DATA |=DispBuffer [3];
			  COM4_ON ;
		    break ;
	}
}

void TimeToDisplayBuffer(void)
{
	uint8_t i,j=0;
	for(i=0;i<2;i++)
	{
		DispBuffer[j++]=table[time[i]%10];
		DispBuffer[j++]=table[time[i]/10];
	}
}





void TIMER32_0_IRQHandler(void)
{
	DisplayScan();
	if(++timeCounter>=500)
	{
		timeCounter=0;
		time_1s_ok=1;
	}
	LPC_TMR32B0->IR=0x01;
}	

void Timer0Init(void)
{
   LPC_SYSCON->SYSAHBCLKCTRL |=(1<<9);
	 LPC_TMR32B0->IR =0x01;
	 LPC_TMR32B0->PR =0;
	 LPC_TMR32B0->MCR=0x03;
	 LPC_TMR32B0->MR0=SystemCoreClock/500;
	 LPC_TMR32B0->TCR=0x01;
	NVIC_EnableIRQ(TIMER_32_0_IRQn);
}




void SEG_IOInit(void)
{
  LPC_GPIO2->DIR |=0xFFF;
}

int main()
{
   SEG_IOInit();
	 Timer0Init();
	 time[1]=00;
	 time[0]=0;
   TimeToDisplayBuffer();
   while(1)
   { 
		 if(time_1s_ok==1)
		 {  
			 time_1s_ok=0;
			 if(++time[0]>=60)
			 {
				 time[0]=0;
				 if(++time[1]>=60)
					 time[1]=0;
			 }
			 TimeToDisplayBuffer();
		 }
	 }		 


}