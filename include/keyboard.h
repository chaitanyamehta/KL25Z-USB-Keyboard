#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdint.h>

void Keyboard_Init(void);
void Keyboard_Keystroke(uint8_t key, uint8_t modifier);
void Keyboard_Print(char *str);

#endif
