//RCC
#define RCC_APB1ENR *((int *)0x40023840)
#define RCC_AHB1ENR *((int *)0x40023830)	
#define RCC_CFGR  *((int *)0x40023808)	
	
//GPIO_B
#define GPIOB_MODER *((int *)0x40020400)
//#define GPIOB_AFRH *((int *)0x40020424)
#define GPIOB_AFRL *((int *)0x40020420)
	
//GPIO-A
#define GPIOA_MODER *((int *)0x40020000)
#define  GPIOA_AFRL *((int *)0x40020020)
	
//Timer-4
#define TIM4_CCMR2 *((int *)0x4000081c)
#define TIM4_CCER *((int *)0x40000820)	
#define TIM4_ARR *((int *)0x4000082c)
#define TIM4_CCR1  *((int *)0x40000834)
#define TIM4_PSC *((int *)0x40000828)
#define TIM4_CR1 *((int *)0x40000800)	
#define TIM4_CNT *((int *)0x40000824)	
#define TIM4_BDTR *((int *)0x40000844)
#define TIM4_EGR  *((int *)0x40000814)	
#define TIM4_SR   *((int *)0x40000810)	
#define TIM4_CCMR1 *((int *)0x40000818)	


// Timer-3
#define TIM3_DIER *((int *)0x4000040c)
#define TIM3_PSC *((int *)0x40000428)
#define TIM3_ARR *((int *)0x4000042c)	
#define TIM3_SR *((int *)0x40000410)	
#define TIM3_CR1 *((int *)0x40000400)	
	
// Timer 2
#define TIM2_PSC *((int *)0x40000028)
#define TIM2_ARR *((int *)0x4000002c)
#define TIM2_CNT *((int *)0x40000024)
#define TIM2_CCMR1 *((int *)0x40000018)
#define TIM2_CCER *((int *)0x40000020)
#define TIM2_CCR1 *((int *)0x40000034)
#define TIM2_CR1  *((int *)0x40000000)

// stk timer
#define STK_CTRL *((int *)0xE000E010)	
#define STK_VAL *((int *)0xE000E018)	
#define STK_LOAD *((int *)0xE000E014)	

void GPIO_Init(void);
void TIM2_Init(void);
void TIM4_ms_Delay(int delay);

void delay(int x)
{
	STK_CTRL |= 0x05;
	STK_LOAD = x*16000;
	STK_VAL = 0x111;
	while((STK_CTRL & (0x1<<16)) == 0)
	{
		;
	}
}

void GPIO_Init(void)
{
		// Timer -2 
    RCC_AHB1ENR |= 1; //Enable GPIOA clock
    GPIOA_AFRL |= (1 << 20); // Select the PA5 pin in alternate function mode
    GPIOA_MODER |= (2 << 10); //Set the PA5 pin alternate function
	
	 // Timer - 4
		RCC_AHB1ENR |= 1 << 1;  //Enable GPIOB clock
		GPIOB_AFRL |= (2 << 24);
		GPIOB_MODER |= (2 << 12);
}

void TIM2_Init()   // to gernarate pwm signal at PA5
{
    RCC_APB1ENR |=1;   // enable timer 2
	
		/*
				psc value = (clk freq /desired freq)-1 . (16 Mhz/1 Mhz)-1=15
		*/	
    TIM2_PSC = 15; //Setting the clock frequency to 1MHz.
	
    TIM2_ARR = 20000; // Total period of the timer 20 ms
	
    TIM2_CNT = 0;
	
	  // PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1
		//else inactive.
		TIM2_CCMR1 &= ~(7 << 4);
    TIM2_CCMR1 |= (3 << 5); //PWM mode for the timer
	
	  //CC1E: Capture/Compare 1 output enable.1: On - OC1 signal is output on the
    //corresponding output pin
    TIM2_CCER |= 1; 
	
    TIM2_CCR1 = 20000 ; // Pulse width for PWM 
}

void TIM4_Init()
{
    RCC_APB1ENR |= 1<<2; //Start the clock for the timer peripheral
	
    TIM4_PSC = 15; //Setting the clock frequency to 1MHz.
	
    TIM4_ARR = 20000; // Total period of the timer
	
    TIM4_CNT = 0;
	
		TIM4_CCMR1 &= ~(7 << 4);
    TIM4_CCMR1 |= (3 << 5); //PWM mode for the timer
	
		TIM4_CCER |= 1; 
	
    TIM4_CCR1 = 20000; //Start the Timer
    
}

int main()
{
  RCC_CFGR |= 0<<10; // set APB1 = 16 MHz
    GPIO_Init();
    TIM2_Init();
		TIM4_Init();
    TIM2_CR1 |= 1;
		TIM4_CR1 |= 1;
		
    while(1)
		{
			TIM2_CCR1 = 15000 ;
			TIM4_CCR1 = 1;   //anticlockwise
			delay(2000);
			TIM2_CCR1 = 0;
			TIM4_CCR1 = 0;
			delay(2000);
			TIM2_CCR1 = 1 ;
			TIM4_CCR1 = 15000;   // clockwise
			delay(2000);
			TIM2_CCR1 = 0;
			TIM4_CCR1 = 0;
			delay(2000);
			
    }
}

