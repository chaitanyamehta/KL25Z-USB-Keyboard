#include <stdbool.h>
#include "usb.h"
#include "usb_keyboard.h"
#include "keyboard.h"

volatile bool wait_flag;

static void keyboard_endpoint_handler(uint8_t endpoint, uint8_t token_pid, buffer_descriptor_t *buffer_descriptor);
uint8_t keyboard_endpoint_rx[2][ENDPOINT1_SIZE];
endpoint_t endpoint1 = {
	.bufferSize = ENDPOINT1_SIZE,
	.rx_even = keyboard_endpoint_rx[0],
	.rx_odd = keyboard_endpoint_rx[1],
	.buffer_in_use = EVEN,
	.data = DATA0,
	.handler = keyboard_endpoint_handler};

static void keyboard_endpoint_handler(uint8_t endpoint, uint8_t token_pid, buffer_descriptor_t *buffer_descriptor)
{
	endpoint_t *ep = USB_GetEndpoint(endpoint);
	switch (token_pid)
	{
	case PID_SETUP:
		break;
	case PID_IN:
		if (ep->tx_data_length > 0)
		{
			endpoint_prepare_next_tx(endpoint);
		}
		wait_flag = false;
		break;
	case PID_OUT:
		break;
	case PID_SOF:
		break;
	default:
		break;
	}
}
	
void Keyboard_Init()
{
	USB_SetEndpoint(1, &endpoint1);
}

void Keyboard_Keystroke(uint8_t key, uint8_t modifier)
{
	wait_flag = true;
	uint8_t buffer[ENDPOINT1_SIZE] = {0};
	
	// Key Press
	buffer[0] = modifier;
	buffer[2] = key;
	endpoint1.tx_data = buffer;
	endpoint1.tx_data_length = ENDPOINT1_SIZE;
	endpoint_prepare_next_tx(1);
	while (wait_flag);
	
	wait_flag = true;
	// Key Release
	buffer[0] = 0;
	buffer[2] = 0;
	endpoint1.tx_data = buffer;
	endpoint1.tx_data_length = ENDPOINT1_SIZE;
	endpoint_prepare_next_tx(1);
	while (wait_flag);
}

void Keyboard_Print(char *str)
{
	uint8_t key, modifier;
	while (*str)
	{
		key = 0;
		modifier = 0;
		char c = *str;
		if (c >= 'a' && c < 'z')
		{
			key = c - 'a' + USB_KEY_A;
		}
		else if (c >= 'A' && c < 'Z')
		{
			modifier |= USB_KEY_MODIFIER_LEFT_SHIFT;
			key = c - 'A' + USB_KEY_A;
		}
		else if (c >= '1' && c < '9')
		{
			key = c - '1' + USB_KEY_1;
		}
		else
		{
			switch (c)
			{
			case '0':
				key = USB_KEY_0;
				break;
			case ' ':
				key = USB_KEY_SPACE;
				break;
			case '\n':
				key = USB_KEY_ENTER;
				break;
			}
		}
		Keyboard_Keystroke(key, modifier);
		str++;
	}
}
