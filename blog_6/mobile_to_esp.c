//GPIO setup
#define AHB1ENR 		*((int *) 0x40023830)  				//access to clock to port A
#define APB1ENR 		*((int *) 0x40023840)	 				//access to USART2
#define APB2ENR 		*((int *) 0x40023844)	
#define GPIOA_MODER *((int *) 0x40020000)			
#define GPIOA_AFRL 	*((int *) 0x40020020)			//altenate function 
	
// USART address setup

#define USART_SR    *((int *)0X40011400)           				//0x40011400 + 0X00
#define USART_DR 		*((int *)0X40011404)
#define USART_BRR 	*((int *)0X40011408) 
#define USART_CR1 	*((int *)0X4001140C) 
#define USART_CR2 	*((int *)0X40011410)
	
//systick timerv

#define STK_CTRL 		*((int *)0xE000E010)	
#define STK_VAL 		*((int *)0xE000E018)	
#define STK_LOAD 		*((int *)0xE000E014)	
	
//port - b

#define GPIOB_MODER *((int *)0x40020400)
#define GPIOB_ODR 	*((int *)0x40020414)
	
// Port - C
#define GPIOC_MODER *((int *)0x40020800)
#define GPIOC_AFRL 	*((int *)0x40020820)
#define GPIOC_IDR 	*((int *)0x40020810) 	
#define GPIOC_PUPDR *((int *)0x4002080c)	

unsigned char h;
char Fetch[10000];

void delay1(int x) 
{
	STK_CTRL |= 0x05;
		STK_LOAD = x*16000;
	STK_VAL = 0x111;
	while((STK_CTRL & (0x1<<16)) == 0)
	{
		;
	}
}

void delay(int x)
{
	int i=0;
	int j=0;
	for(;j<x;j++)
	{
		for(;i<833333;i++)
		{
			;
		}
	}
}

void init()
{	
	AHB1ENR |= 0x4;	
	while(!(AHB1ENR & 4)){;}				//clock to port C
		
	APB2ENR     |= (1<<5);					//set 1 at USART6EN
	while(!(APB2ENR &(1<<5)));
}

void config()
{
	GPIOC_MODER |= (0xa<<12);				// set '10' for alternative 6 & 7 port
	GPIOC_AFRL  |= 0x88000000;					// set '1000' in AFRL6 , set '1000' in AFRL7
}

void UART_config()
{
	USART_BRR |= (0x8B);						// set baud rate to 112500
	USART_CR1 |= (0x3<<2);					// set RE and TE (transmitt enable and receive enable)
	USART_CR1 |= (0x1<<13);					// set UE (usart enable)
}

void UART_Tx(unsigned char x)
{
	while(!(USART_SR & (1<<7)));			// check TXE
	USART_DR=x;
}

void UART2_OutString(char* s)
{
	int i=0;
	for(;s[i];i++)
		UART_Tx(s[i]);
}


int main ()
{
	init();
	config();
	UART_config();
	UART2_OutString("AT\r\n"); 
	delay(2000);
	
	UART2_OutString("AT+CWMODE=2\r\n"); 
	delay(2000);
	
	UART2_OutString("AT+CWSAP=\"fone\",\"12345678\",1,4,4,0\r\n"); 
	delay(3000);
	
	UART2_OutString("AT+CIFSR\r\n"); 
	delay(2000);   //to find IP of access point
	
	UART2_OutString("AT+CWLIF\r\n"); 
	delay(2000);   //to find IP of station point connected to access point     
		
	UART2_OutString("AT+CIPMUX=1\r\n"); 
	delay(2000);
	while(1)
	{	
		UART2_OutString("AT+CIPSERVER=1,80\r\n"); 
		delay(2000);  // search in mobile
		
		UART2_OutString("AT+CIFSR\r\n"); 
                delay(2000);   //to find IP of access point
	
		UART2_OutString("AT+CIPSTATUS\r\n"); 
		delay(2000);
	
		UART2_OutString("AT+CIPSEND=0,14\r\n"); 
		delay(2000);
	
  		UART2_OutString("Hello World!\r\n"); 
		delay(2000);
		
		UART2_OutString("AT+CIPCLOSE=0\r\n");
		delay(2000);
	}
}


