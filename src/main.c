#include <MKL25Z4.H>
#include <cmsis_os2.h>
#include "usb.h"
#include "led.h"
#include "thread.h"

int main(void)
{
	RGB_LED_Init();
	Control_RGB(0, 0, 0);
	
	USB_Init();
	
	osKernelInitialize();
	Create_OS_Objects();
	osKernelStart();	
	while(1);
}
