#include <MKL25Z4.h>
#include "led.h"

void RGB_LED_Init(void)
{
    // Enable clock to ports B and D
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;

    // Make 3 pins GPIO
    PORTB->PCR[LED_RED_POS] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[LED_RED_POS] |= PORT_PCR_MUX(1);
    PORTB->PCR[LED_GREEN_POS] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[LED_GREEN_POS] |= PORT_PCR_MUX(1);
    PORTD->PCR[LED_BLUE_POS] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[LED_BLUE_POS] |= PORT_PCR_MUX(1);

    // Set ports to outputs
    PTB->PDDR |= LED_RED_MASK | LED_GREEN_MASK;
    PTD->PDDR |= LED_BLUE_MASK;
}

void Control_RGB(bool red, bool green, bool blue)
{
    if (red)
        PTB->PCOR = LED_RED_MASK;
    else
        PTB->PSOR = LED_RED_MASK;
    if (green)
        PTB->PCOR = LED_GREEN_MASK;
    else
        PTB->PSOR = LED_GREEN_MASK;
    if (blue)
        PTD->PCOR = LED_BLUE_MASK;
    else
        PTD->PSOR = LED_BLUE_MASK;
}
