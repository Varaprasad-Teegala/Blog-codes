#define RCC_AHB1ENR *((int *)0X40023830) //RCC base address + AHBIENR offset address (refer page 38) 
#define GPIO_MODER *((int *)0x40020400)
#define GPIO_ODR *((int *)0x40020414)
	
//systick timer

#define STK_CTRL *((int *)0xE000E010)	
#define STK_VAL *((int *)0xE000E018)	
#define STK_LOAD *((int *)0xE000E014)	
//unsigned char y='A';
void clock()
{
	RCC_AHB1ENR |= 0X2;
	while(!(RCC_AHB1ENR & 0x2 ))
	{
		;
	}
}

void delay(int x)
{
//	STK_CTRL &= ~(7);
	STK_CTRL |= 0x05;
		STK_VAL = 16000;
	STK_LOAD = x*16000;
	while((STK_CTRL & (0x1<<16)) == 0)
	{
		;
	}
}

void modset()
{
	GPIO_MODER &= 0xfffcf000;
	GPIO_MODER |= 0x00010555;
}

/*void write_high_nibble( unsigned char data );
void write_low_nibble( unsigned char data );*/

void KM_LCD_Write_Cmd( unsigned char x)
{
	unsigned char p = x;
	GPIO_ODR &= ~(0x1<<4);//RS = COMMAND CLEAR
	
	p=p<<4;
	p=p>>4;
	x=x>>4;
	//x=x<<4;
	GPIO_ODR |= (x);//HIGHER NIBBLE
	GPIO_ODR |= (0x100); 			//enable =1
	delay(20);
	GPIO_ODR &= ~(0x100);			// enable=0
	GPIO_ODR &= ~(x);
	//GPIO_ODR &= (0XFFFFFFF0);
	
	GPIO_ODR |= (p);//
	GPIO_ODR |= (0x100); 			//enable =1
	delay(20);
	GPIO_ODR &= ~(0x100);			// enable=0
	GPIO_ODR &= (0XFFFFFFF0);
	
	
}
void KM_LCD_Init(void)
{
	delay(20);
	KM_LCD_Write_Cmd( 0x33 );
	delay(1);
	KM_LCD_Write_Cmd( 0x32 );
	KM_LCD_Write_Cmd( 0x0c );
	KM_LCD_Write_Cmd( 0x01 );
}

void KM_LCD_Write_Data( unsigned char x)
{
	unsigned char p,q;
	GPIO_ODR |= (0x1<<4);
	
	p=q=x;
	q=q<<4;
	q=q>>4;
	p=p>>4;
	
	GPIO_ODR |= (p);
	GPIO_ODR |= (0x100); 			//enable =1
	delay(20);
	GPIO_ODR &= ~(0x100);			// enable=0
	GPIO_ODR &= ~(p);
	GPIO_ODR &= ~(1<<4);			//disable RS
	
	GPIO_ODR |= (q);
	GPIO_ODR |= (1<<4);				// enable RS
	GPIO_ODR |= (0x100); 			//enable =1
	delay(20);
	GPIO_ODR &= ~(0x100);			// enable=0
	GPIO_ODR &= ~(q);
	
	
}
void KM_LCD_Write_Str(char *); 

int main()
{
	clock();
	modset();
	KM_LCD_Init();
	KM_LCD_Write_Data('A');
	while(1)
	{
		GPIO_ODR &= ~(0x1<<13);
		delay(100);
		GPIO_ODR |= (0x1<<13);
		delay(100); 
		
	}		
	return 0;
	
}


