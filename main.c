#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

#include "led.h"
#include "serial.h"
#include "timer.h"
#include "adc.h"
#include "dht.h"

uint8_t I_RH, D_RH, I_Temp, D_Temp, CheckSum;
int dutycycle = 0;
void set_brightness_red(uint8_t data)
{
	OCR0A = data;
}
void set_brightness_blue(uint8_t data)
{
	OCR2A = data;
}

ISR(TIMER0_OVF_vect)
{
	OCR0A = (dutycycle / 100.0) * 255;
}

int main(void)
{
	char data[5];
	uint8_t brightness[3] = { 20,150,255};
	uint8_t brightness1[3] = {20,150,255};
	led_init();
	timer0_init();
	timer2_init();
    uart_init();
	set_brightness_blue(100);
	set_brightness_red(100);
	while (1)
	{
		_delay_ms(2000);
		Request();		/* send start pulse */
		Response();		/* receive response */
		I_RH = Receive_data();	/* store first eight bit in I_RH */
		D_RH = Receive_data();	/* store next eight bit in D_RH */
		I_Temp = Receive_data();	/* store next eight bit in I_Temp */
		D_Temp = Receive_data();	/* store next eight bit in D_Temp */
		CheckSum = Receive_data();/* store next eight bit in CheckSum */
		int RH = I_RH + D_RH;
		int TEMP = I_Temp;

		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			printf("Error\r\n");
		}
		else
		{
			itoa(I_RH, data, 10);
			printf("Humidity = %s", data);
			itoa(D_RH, data, 10);
			printf(".%s\n", data);

			itoa(I_Temp, data, 10);
			printf("Temperature = %s", data);
			itoa(D_Temp, data, 10);
			printf(".%s C\n", data);

			if (RH >= 60)
			{
				dutycycle = 200;
				/*set_brightness_red(brightness[2]);*/
	
			}
			else if (RH >= 50 && RH <= 59)
			{
				set_brightness_red(brightness[1]);
			}
			else if (RH <= 49)
			{
				set_brightness_red(brightness[0]);
			}
			if ( TEMP>= 30)
			{
				set_brightness_blue(brightness1[2]);
			}
			else if (TEMP >= 24 && TEMP <= 29)
			{
				set_brightness_blue(brightness1[1]);
			}
			else if (TEMP <=23)
			{
				set_brightness_blue(brightness1[0]);
			}
		}
	}
    return 0;
}

