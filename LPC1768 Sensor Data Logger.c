#include <LPC17xx.h>
#include <stdio.h>

#define LCD_DATA_MASK   (0xFF << 16)

#define LCD_RS          (1 << 24)
#define LCD_EN          (1 << 25)

#define LM35_CHANNEL    0       /* AD0.0 -> P0.23 */
#define LDR_CHANNEL     1       /* AD0.1 -> P0.24 */

void delay_ms(unsigned int ms)
{
   unsigned int i, j;

   for(i = 0; i < ms; i++)
   {
      for(j = 0; j < 5000; j++)
      {
         __NOP();
      }
   }
}

void UART0_Init(void)
{
   LPC_PINCON->PINSEL0 &= ~((3 << 4) | (3 << 6));    /*P0.2 = TXD0 P0.3 = RXD0 */
   LPC_PINCON->PINSEL0 |=  ((1 << 4) | (1 << 6));

   LPC_SC->PCONP |= (1 << 3);
   LPC_UART0->LCR = 0x83;
   LPC_UART0->DLM = 0;
   LPC_UART0->DLL = 163;
   LPC_UART0->LCR = 0x03;
}

void UART0_SendChar(char ch)
{
   while(!(LPC_UART0->LSR & (1 << 5)));

   LPC_UART0->THR = ch;
}

void UART0_SendString(char *str)
{
   while(*str)
   {
      UART0_SendChar(*str);
      str++;
   }
}

void ADC_Init(void)
{

   LPC_PINCON->PINSEL1 &= ~((3 << 14) | (3 << 16));
   LPC_PINCON->PINSEL1 |= ((1 << 14) | (1 << 16));

   LPC_SC->PCONP |= (1 << 12);
   LPC_ADC->ADCR = (1 << 21) | (4 << 8);         /* CLKDIV */
}

unsigned int ADC_Read(unsigned int channel)
{
   unsigned int data;
   LPC_ADC->ADCR &= ~(0xFF);

   LPC_ADC->ADCR |= (1 << channel);
   LPC_ADC->ADCR |= (1 << 24);

   while(!(LPC_ADC->ADGDR & (1 << 31)));

   LPC_ADC->ADCR &= ~(7 << 24);

   data = (LPC_ADC->ADGDR >> 4) & 0xFFF;

   return data;
}

void LCD_Enable(void)
{
   LPC_GPIO1->FIOSET = LCD_EN;

   delay_ms(1);

   LPC_GPIO1->FIOCLR = LCD_EN;

   delay_ms(1);
}

void LCD_Command(unsigned char cmd)
{
   LPC_GPIO1->FIOCLR = LCD_DATA_MASK;
   LPC_GPIO1->FIOSET = ((unsigned int)cmd << 16);
   LPC_GPIO1->FIOCLR = LCD_RS;
   LCD_Enable();
}

void LCD_Data(unsigned char data)
{
   LPC_GPIO1->FIOCLR = LCD_DATA_MASK;
   LPC_GPIO1->FIOSET = ((unsigned int)data << 16);
   LPC_GPIO1->FIOSET = LCD_RS;

   LCD_Enable();
}

void LCD_Init(void)
{

   LPC_GPIO1->FIODIR |= LCD_DATA_MASK | LCD_RS | LCD_EN;

   delay_ms(20);

   LCD_Command(0x38);     /* 8-bit, 2-line */
   LCD_Command(0x0C);     /* Display ON */
   LCD_Command(0x06);     /* Entry mode */
   LCD_Command(0x01);     /* Clear display */

   delay_ms(5);
}

void LCD_String(char *str)
{
   while(*str)
   {
     LCD_Data(*str);
     str++;
   }
}

void LCD_SetCursor(unsigned char row,unsigned char column)
{
   unsigned char address;

   if(row == 0)
     address = 0x80 + column;
   else
     address = 0xC0 + column;

   LCD_Command(address);
}

int main(void)
{
   unsigned int temperature_adc;
   unsigned int light_adc;

   unsigned int temperature;
   unsigned int light;

   char buffer[50];

   UART0_Init();

   ADC_Init();

   LCD_Init();

   UART0_SendString("\r\nLPC1768 Sensor Data Logger\r\n");

   UART0_SendString("---------------------------\r\n");

   LCD_SetCursor(0,0);
   LCD_String("Sensor Logger");

   delay_ms(2000);

   LCD_Command(0x01);


   while(1)
   {
      temperature_adc = ADC_Read(LM35_CHANNEL);
      light_adc = ADC_Read(LDR_CHANNEL);

      temperature = (temperature_adc * 3300) / 4095;

      temperature =temperature / 10;

      light =(light_adc * 100) / 4095;
      LCD_Command(0x01);
      LCD_SetCursor(0,0);
      sprintf(buffer,"Temp: %d C",temperature);

      LCD_String(buffer);
      LCD_SetCursor(1,0);

      sprintf(buffer,"Light: %d%%",light);

      LCD_String(buffer);

      sprintf(buffer,"Temperature = %d C\r\n",temperature);

      UART0_SendString(buffer);
      sprintf(buffer,"Light Level = %d %%\r\n",light);

      UART0_SendString(buffer);

      UART0_SendString("----------------------\r\n");
      delay_ms(2000);
   }
}