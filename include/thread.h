#ifndef __THREAD_H
#define __THREAD_H

#include <cmsis_os2.h>

// Thread Period (1 tick/ms)
#define THREAD_USB_KEYBOARD_PERIOD_MS (10000)

// Thread Priority
#define THREAD_USB_KEYBOARD_PRIORITY 		osPriorityNormal

void Create_OS_Objects(void);

#endif
