#include <MKL25Z4.H>
#include <stddef.h>
#include "usb.h"
#include "usb_keyboard.h"
#include "led.h"
#include "delay.h"

static void endpoint1_handler(uint8_t endpoint, uint8_t token_pid, buffer_descriptor_t *buffer_descriptor);
uint8_t endpoint1_rx[2][ENDPOINT1_SIZE];
endpoint_t endpoint1 = {
	.bufferSize = ENDPOINT1_SIZE,
	.rx_even = endpoint1_rx[0],
	.rx_odd = endpoint1_rx[1],
	.buffer_in_use = EVEN,
	.data = DATA0,
	.handler = endpoint1_handler};

static void print(char *str);

int main(void)
{
	RGB_LED_Init();
	Control_RGB(0, 0, 0);

	USB_Init();
	USB_SetEndpoint(1, &endpoint1);

	delay(1000);
	print("Hello World");
	while (1)
	{
	}
}

bool wait_flag;
static void send(uint8_t *buffer)
{
	wait_flag = true;
	endpoint1.tx_data = buffer;
	endpoint1.tx_data_length = ENDPOINT1_SIZE;
	endpoint_prepare_next_tx(1);
	while (wait_flag)
		;
}

static void print(char *str)
{
	uint8_t buffer[2][ENDPOINT1_SIZE] = {0};
	while (*str)
	{
		char c = *str;
		buffer[0][0] = 0;
		buffer[0][2] = 0;
		if (c >= 'a' && c < 'z')
		{
			buffer[0][2] = c - 'a' + USB_KEY_A;
		}
		else if (c >= 'A' && c < 'Z')
		{
			buffer[0][0] |= USB_KEY_MODIFIER_LEFT_SHIFT;
			buffer[0][2] = c - 'A' + USB_KEY_A;
		}
		else if (c >= '1' && c < '9')
		{
			buffer[0][2] = c - '1' + USB_KEY_1;
		}
		else
		{
			switch (c)
			{
			case '0':
				buffer[0][2] = USB_KEY_0;
				break;
			case ' ':
				buffer[0][2] = USB_KEY_SPACE;
				break;
			}
		}
		send(buffer[0]);
		send(buffer[1]);
		str++;
	}
}

static void endpoint1_handler(uint8_t endpoint, uint8_t token_pid, buffer_descriptor_t *buffer_descriptor)
{
	endpoint_t *ep = USB_GetEndpoint(endpoint);
	switch (token_pid)
	{
	case PID_SETUP:
		Control_RGB(0, 0, 1);
		break;
	case PID_IN:
		if (ep->tx_data_length > 0)
		{
			endpoint_prepare_next_tx(endpoint);
		}
		wait_flag = false;
		break;
	case PID_OUT:
		Control_RGB(0, 1, 1);
		break;
	case PID_SOF:
		Control_RGB(1, 0, 0);
		break;
	default:
		Control_RGB(1, 0, 1);
		break;
	}
}
