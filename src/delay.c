#include "delay.h"

void delay(uint32_t delay)
{
	volatile uint32_t t;
	for (t = delay * 10000; t > 0; t--)		;
}
