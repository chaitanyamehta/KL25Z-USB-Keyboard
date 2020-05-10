#ifndef __LED_H
#define __LED_H

#include <stdbool.h>

#define LED_RED_POS (18)   // PORTB
#define LED_GREEN_POS (19) //PORTB
#define LED_BLUE_POS (1)   //PORTD

#define LED_RED_MASK (1 << 18)   // PORTB
#define LED_GREEN_MASK (1 << 19) //PORTB
#define LED_BLUE_MASK (1 << 1)   //PORTD

void RGB_LED_Init(void);
void Control_RGB(bool red, bool green, bool blue);

#endif
