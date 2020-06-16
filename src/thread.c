#include <cmsis_os2.h>
#include "thread.h"
#include "keyboard.h"

void Thread_USB_Keyboard(void *arg);

osThreadId_t t_USB_Keyboard;

// Thread Attributes

const osThreadAttr_t USB_Keyboard_attr = {
  .priority = THREAD_USB_KEYBOARD_PRIORITY            
};

void Create_OS_Objects(void) 
{
	t_USB_Keyboard = osThreadNew(Thread_USB_Keyboard, NULL, &USB_Keyboard_attr);
}

void Thread_USB_Keyboard(void *arg)
{
	Keyboard_Init();
	while(1)
	{
		Keyboard_Print("Hello World\n");
		osDelay(THREAD_USB_KEYBOARD_PERIOD_MS);
	}		
}
