#define AHB1ENR *((int *)0x40023830)
#define APB2ENR *((int *)0x40023844)	
	
// Port - B

#define GPIOB_MODER *((int *)0x40020400)
#define GPIOB_ODR *((int *)0x40020414)
    
// Port - C
#define GPIOC_MODER *((int *)0x40020800)
#define GPIOC_IDR *((int *)0x40020810)  
#define GPIOC_PUPDR *((int *)0x4002080c)  

//systic timer
#define STK_CTRL *((int *)0xE000E010)	
#define STK_VAL *((int *)0xE000E018)	
#define STK_LOAD *((int *)0xE000E014)	
	
//External interrupt
#define SYSCFG_EXTICR3 *((int *) 0x40013810)
#define EXTI_PR *((int *)0x40013c14)	
#define EXTI_IMR *((int *)0x40013c00)
#define EXT_FTSR *((int *)0x40013c0c)

//NVIC 
#define NVIC_ISER0 *((int *)0XE000E100)
int count =0;
void EXTI15_10_IRQHandler();
void init()
{
        AHB1ENR |= 0x2;     // enable port B
        AHB1ENR |= 0x4;			// enable port C
        while(!(AHB1ENR & 2)) 
        {
                ;
        }
        while(!(AHB1ENR & 4)) 
        {
                ;
        }
}

void config()
{
	// Port _ B config
	GPIOB_MODER &= 0xc3ffffff;						
	GPIOB_MODER |= 0x14000000;						// set port-B 14th pin to '01' config which means to produce output (14th pin is connected to my LED)
	while(!(GPIOB_MODER & 0x14000000))
	{
		;
	}
	// Port - C config
	GPIOC_MODER &= 0xfff0ffff;						//  set port-c 8th pin to '00' config,which means to take input (8th pin is connected to my push button )
	//GPIOC_MODER |= 0x00000;
	while((GPIOC_MODER & 0x00000))
	{
		;
	}
	
	GPIOC_PUPDR &= 0xfff0ffff;
	GPIOC_PUPDR |= 0x50000;               // pull up config,when button is pressed '0' bit is produced (falling edge)
	while(!(GPIOC_PUPDR & 0x50000))
	{
		;	
	}	
	GPIOB_ODR |= (3<<13);								
}


void delay(int x)
{
	STK_CTRL &= ~(7);
	STK_CTRL |= 0x05;
	STK_LOAD = x*16000;
	STK_VAL = 0x111;
	while((STK_CTRL & (0x1<<16)) == 0)
	{
		;
	}
	
}

void EXT_config()
{
	APB2ENR &= ~(14<<1);
	APB2ENR |= (0x1<<14);           // enable SYSCFG
	SYSCFG_EXTICR3 &= ~(0x0022);			 
	SYSCFG_EXTICR3 |= 0x0022;				// enable system clock to interrupts .Config '0010' for interrupt at port-c 8th pin where my button is connected
	EXTI_IMR &= ~(3<<8);					
	EXTI_IMR |= (3<<8);							// set '1' to request an interrupt
	EXT_FTSR &= ~(3<<8);						
	EXT_FTSR |= (3<<8);							// Falling trigger enabled for 8th pin.
	NVIC_ISER0 &= ~(0x00800000);		
	NVIC_ISER0 |= (0x00800000);			// from vector table EXTI[5:9] port have 23 priority ,so set '1' at 23rd bit in NVIC registers
}

int main()
{
        init();
        config();
	EXT_config();

        while(1)
        {}
        return 0;
}

void EXTI9_5_IRQHandler()
{
		if(EXTI_PR & (1<<8))    // whenever we press the button, 8th bit of EXTI_PR register is automatically set to '1'.when it is SET check it
		{
			GPIOB_ODR ^= (1<<13); // Toggle LED.
			count++;
			EXTI_PR |= (1<<8);
		}
}



