#include  <msp430.h>

void delay(int);
void display_number(int, int);
void display_time();
void increament_sec();
void increament_hour();
void increament_minute();
void config_interrupt();

short hour_tens=1, hour_ones=1, minute_tens=5, minute_ones=9, second=0;

short blink=0, blink_minute=0, blink_hour=0, enable_hour_blink=0, enable_minute_blink=0;

int arr[10]={0x7e,0x48,0x3d,0x6d,0x4b,0x67,0x77,0x4c,0x7f,0x6f};

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

	P2DIR = BIT0+BIT1+BIT2+BIT3; 	// P2 output
	P2OUT = BIT0; 					// P2 output

	// Configure Switch on P2
	P2REN |= BIT4 + BIT5; 			// P2 Enable Pullup/Pulldown
	P2OUT = BIT4 + BIT5; 			// P2 pullup
	P2IE |= BIT4 + BIT5; 			// P2 interrupt enabled
	P2IES |= BIT4 + BIT5; 			// P2 Hi/lo falling edge
	P2IFG &= ~(BIT4 + BIT5); 		// P2IFG cleared just in case

	_EINT();

	P1DIR |= 0xff;					// set P1 as output
	P1OUT = 0x01;					// P1 low

	BCSCTL1 |= DIVA_0;				// ACLK/1
	BCSCTL3 |= XCAP_3 + LFXT1S_0;	//12.5pF cap- setting for 32768Hz crystal

	CCTL0 = CCIE;					// CCR0 interrupt enabled
	CCR0 = 32767;						// 512 -> 1 sec, 30720 -> 1 min
	TACTL = TASSEL_1 + ID_0 + MC_1;			// ACLK, /8, upmode

	_BIS_SR(GIE);					// Enter interrupt

	while(1)
	{
		display_time();
	}
}

void config_interrupt()
{
	P2REN |= BIT4 + BIT5; // P2 Enable Pullup/Pulldown
	P2OUT |= BIT4 + BIT5; // P2 pullup
	P2IE |= BIT4 + BIT5; // P2 interrupt enabled
	P2IES |= BIT4 + BIT5; // P2 Hi/lo falling edge
	P2IFG &= ~(BIT4 + BIT5); // P2 IFG cleared just in case
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	if((P2IFG & 0x20) == 0x20)
	{
		P2IE &= ~BIT5;

		if(enable_hour_blink==0 && enable_minute_blink==0)
		{
			enable_minute_blink=1;
			enable_hour_blink=0;
		}
		else if(enable_hour_blink==0 && enable_minute_blink==1)
		{
			enable_minute_blink=0;
			enable_hour_blink=1;
			blink_minute=0;
		}
		else if(enable_hour_blink==1 && enable_minute_blink==0)
		{
			enable_minute_blink=0;
			enable_hour_blink=0;
		}

		P2IE |= BIT5;
		P2IFG &= ~BIT5;
	}
	else if((P2IFG & 0x10) == 0x10)
	{
		P2IE &= ~BIT4;

		if(enable_hour_blink)
			increament_hour();
		else if(enable_minute_blink)
			increament_minute();

		P2IE |= BIT4;
		P2IFG &= ~BIT4;
	}
}


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	increament_sec();

	if(blink) blink=0;
	else blink=1;

	if(enable_hour_blink)
	{
		if(blink_hour)
			blink_hour=0;
		else
			blink_hour=1;
	}

	if(enable_minute_blink)
	{
		if(blink_minute) blink_minute=0;
		else blink_minute=1;
	}

	if(enable_hour_blink==0 && enable_minute_blink==0)
	{
		blink_hour=0;
		blink_minute=0;
	}
}

void delay(int i)
{
	for(;i>0;i--)
	{
		_delay_cycles(100);
	}
}

void display_number(int i, int j)
{
	P1OUT=0x00;

	P2OUT=(0xf0 | j);

	P1OUT=arr[i];

	if(j==1 && blink)
	{
		P1OUT |= 0x80;
	}
	if((j==4 || j==8) && blink_hour==1)
	{
		P1OUT = 0x00;
	}
	else if((j==1 || j==2) && blink_minute==1)
	{
		P1OUT = 0x00;
	}
}

void display_time()
{
	delay(5);
	display_number(minute_ones,1);

	delay(5);
	display_number(minute_tens,2);

	delay(5);
	display_number(hour_ones,4);

	delay(5);
	display_number(hour_tens,8);
}

void increament_hour()
{
	hour_ones++;
	second=0;

	if(hour_ones==10)
	{
		hour_ones=0;
		hour_tens++;
	}
	if(hour_tens==1)
	{
		if(hour_ones==2)
		{
			hour_tens=0;
			hour_ones=0;
			minute_tens=0;
			minute_ones=0;
			second=0;
		}
	}
}

void increament_minute()
{
	minute_ones++;
	second=0;

	if(minute_ones==10)
	{
		minute_ones=0;
		minute_tens++;
	}
	if(minute_tens==6)
	{
		minute_tens=0;
		hour_ones++;
	}
	if(hour_ones==10)
	{
		hour_ones=0;
		hour_tens++;
	}
	if(hour_tens==1)
	{
		if(hour_ones==2)
		{
			hour_tens=0;
			hour_ones=0;
			minute_tens=0;
			minute_ones=0;
			second=0;
		}
	}
}

void increament_sec()
{
	second++;

	if(second==60)
	{
		second=0;
		minute_ones++;
	}
	if(minute_ones==10)
	{
		minute_ones=0;
		minute_tens++;
	}
	if(minute_tens==6)
	{
		minute_tens=0;
		hour_ones++;
	}
	if(hour_ones==10)
	{
		hour_ones=0;
		hour_tens++;
	}
	if(hour_tens==1)
	{
		if(hour_ones==2)
		{
			hour_tens=0;
			hour_ones=0;
			minute_tens=0;
			minute_ones=0;
			second=0;
		}
	}
}
