#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <MKL25Z4.h>
#include "usb.h"
#include "usb_spec.h"
#include "buffer_descriptor.h"
#include "usb_descriptors.h"

__attribute((aligned(512))) buffer_descriptor_t buffer_descriptor_table[NUM_BUFFER_DESCRIPTORS];

static void init_buffer_descriptor(uint8_t endpoint_index);
static void endpoint0_handler(uint8_t endpoint, uint8_t token_pid, buffer_descriptor_t *buffer_descriptor);
static bool endpoint_setup(endpoint_t *endpoint, usb_setup_packet_t *setup);
void endpoint_prepare_next_tx(uint8_t endpoint);
void buffer_descriptor_rx_release(buffer_descriptor_t *buffer_descriptor, uint8_t bufferSize);

// Endpoint 0 even and odd buffers
uint8_t endpoint0_rx[2][ENDPOINT0_SIZE];
endpoint_t endpoint0 = {
	.bufferSize = ENDPOINT0_SIZE,
	.rx_even = endpoint0_rx[0],
	.rx_odd = endpoint0_rx[1],
	.buffer_in_use = EVEN,
	.data = DATA0,
	.handler = endpoint0_handler};

endpoint_t *endpoints[NUM_ENDPOINTS] = {&endpoint0, NULL};

void USB_SetEndpoint(uint8_t index, endpoint_t *endpoint)
{
	if (index > 0 && index < NUM_ENDPOINTS)
	{
		endpoints[index] = endpoint;
		init_buffer_descriptor(index);
	}
}

endpoint_t *USB_GetEndpoint(uint8_t index)
{
	if (index > 0 && index < NUM_ENDPOINTS)
	{
		return endpoints[index];
	}
	return NULL;
}

void USB_Init(void)
{
	// Select clock source
	SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK;

	// USB clock gating
	SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;

	// Reset USB module (software)
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	while (USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK)
		;

	// Set BDT base registers
	/* BDTPAGEx provides the base address where the current Buffer Descriptor Table (BDT) resides in system memory */
	USB0->BDTPAGE1 = ((uint32_t)buffer_descriptor_table) >> 8;
	USB0->BDTPAGE2 = ((uint32_t)buffer_descriptor_table) >> 16;
	USB0->BDTPAGE3 = ((uint32_t)buffer_descriptor_table) >> 24;

	// Clear all USB ISR flags and enable weak pull-downs
	USB0->ISTAT = 0xFF;
	USB0->ERRSTAT = 0xFF;
	USB0->OTGISTAT = 0xFF;
	USB0->USBTRC0 |= 0x40;

	USB0->CTL = USB_CTL_USBENSOFEN_MASK;
	USB0->USBCTRL = USB_USBCTRL_PDE_MASK;

	// Enable USB reset interrupt
	USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;
	NVIC_EnableIRQ(USB0_IRQn);

	// Enable pull up resistor
	USB0->CONTROL = USB_CONTROL_DPPULLUPNONOTG_MASK;
}

void USB0_IRQHandler(void)
{
	uint8_t status;

	status = USB0->ISTAT;

	if (status & USB_ISTAT_USBRST_MASK)
	{
		// Reset all Endpoints
		// Configure Endpoint 0
		USB0->CTL |= USB_CTL_ODDRST_MASK;
		init_buffer_descriptor(0);

		USB0->ADDR = 0;

		// Clear all USB Flags
		USB0->ISTAT = 0xFF;
		USB0->ERRSTAT = 0xFF;

		// Enable USB interupt sources
		USB0->ERREN = 0xFF;
		USB0->INTEN = USB_INTEN_USBRSTEN_MASK | USB_INTEN_ERROREN_MASK |
					  USB_INTEN_SOFTOKEN_MASK | USB_INTEN_TOKDNEEN_MASK |
					  USB_INTEN_SLEEPEN_MASK | USB_INTEN_STALLEN_MASK;
		return;
	}
	if (status & USB_ISTAT_ERROR_MASK)
	{
		// handle error
		uint8_t error_src = USB0->ERRSTAT;
		USB0->ERRSTAT = error_src;
		USB0->ISTAT = USB_ISTAT_ERROR_MASK;
	}
	if (status & USB_ISTAT_SOFTOK_MASK)
	{
		// handle start of frame token
		USB0->ISTAT = USB_ISTAT_SOFTOK_MASK;
	}
	if (status & USB_ISTAT_TOKDNE_MASK)
	{
		// handle completion of current token being processed
		uint8_t stat = USB0->STAT;
		uint8_t endpoint = stat >> 4;
		uint8_t direction = (stat & USB_STAT_TX_MASK) >> USB_STAT_TX_SHIFT;
		uint8_t destination = (stat & USB_STAT_ODD_MASK) >> USB_STAT_ODD_SHIFT;

		buffer_descriptor_t *buffer_descriptor = &buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint, direction, destination)];
		uint8_t token_pid = BUFFER_DESCRIPTOR_TOK_PID(buffer_descriptor->fields);
		endpoint_t *ep = endpoints[endpoint];

		if (ep->handler != NULL)
		{
			ep->handler(endpoint, token_pid, buffer_descriptor);
		}

		if (direction == RX)
		{
			buffer_descriptor_rx_release(buffer_descriptor, ep->bufferSize);
		}
		USB0->ISTAT = USB_ISTAT_TOKDNE_MASK;
	}

	if (status & USB_ISTAT_SLEEP_MASK)
	{
		// handle USB sleep
		USB0->ISTAT = USB_ISTAT_SLEEP_MASK;
	}

	if (status & USB_ISTAT_STALL_MASK)
	{
		// handle USB stall
		USB0->ISTAT = USB_ISTAT_STALL_MASK;
	}
}

static void init_buffer_descriptor(uint8_t endpoint_index)
{
	endpoint_t *endpoint = endpoints[endpoint_index];
	uint8_t bufferSize = endpoint->bufferSize;
	endpoint->buffer_in_use = EVEN;
	endpoint->data = DATA0;
	buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint_index, RX, EVEN)].fields = BUFFER_DESCRIPTOR(bufferSize, DATA0);
	buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint_index, RX, EVEN)].bufferAddress = (void *)endpoint->rx_even;
	buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint_index, RX, ODD)].fields = BUFFER_DESCRIPTOR(bufferSize, DATA1);
	buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint_index, RX, ODD)].bufferAddress = (void *)endpoint->rx_odd;
	buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint_index, TX, EVEN)].fields = 0;
	buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint_index, TX, ODD)].fields = 0;
	USB0->ENDPOINT[endpoint_index].ENDPT = USB_ENDPT_EPRXEN_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPHSHK_MASK;
}

static void endpoint0_handler(uint8_t endpoint, uint8_t token_pid, buffer_descriptor_t *buffer_descriptor)
{
	static usb_setup_packet_t setup;
	endpoint_t *ep = endpoints[endpoint];
	switch (token_pid)
	{
	case PID_SETUP:
		// extract setup token
		memcpy((void *)&setup, buffer_descriptor->bufferAddress, sizeof(usb_setup_packet_t));

		buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint, TX, EVEN)].fields = 0;
		buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint, TX, ODD)].fields = 0;
		ep->data = DATA1;

		if (endpoint_setup(ep, &setup))
		{
			USB0->ENDPOINT[endpoint].ENDPT = USB_ENDPT_EPSTALL_MASK |
											 USB_ENDPT_EPRXEN_MASK |
											 USB_ENDPT_EPTXEN_MASK |
											 USB_ENDPT_EPHSHK_MASK;
		}
		else
		{
			/* truncate data length */
			if (ep->tx_data_length > setup.wLength)
			{
				ep->tx_data_length = setup.wLength;
			}
			/*prepare two buffers with the answer*/
			endpoint_prepare_next_tx(endpoint);
			if (ep->tx_data_length > 0)
			{
				endpoint_prepare_next_tx(endpoint);
			}
		}

		// unfreeze
		USB0->CTL = USB_CTL_USBENSOFEN_MASK;
		break;
	case PID_IN:
		// send any pending transmit data
		if (ep->tx_data_length > 0)
		{
			endpoint_prepare_next_tx(endpoint);
		}

		// set address
		if (setup.request.wRequestAndType == 0x0500)
		{
			USB0->ADDR = setup.wValue;
		}
		break;
	case PID_OUT:
		break;
	case PID_SOF:
		break;
	}
}

static bool endpoint_setup(endpoint_t *endpoint, usb_setup_packet_t *setup)
{
	bool stall = false;
	switch (setup->request.wRequestAndType)
	{
	case 0x0500: // set address (wait for IN packet)
		break;
	case 0x0900: // set configuration
		break;
	case 0x0680: // get descriptor
	case 0x0681:
		if ((setup->wValue & (USB_DESCRIPTOR_TYPE_STRING << 8)) == (USB_DESCRIPTOR_TYPE_STRING << 8))
		{
			uint8_t index = setup->wValue & 0x00FF;
			if (index < STRING_DESCRIPTORS)
			{
				endpoint->tx_data = (uint8_t *)string_descriptors[index].descriptor;
				endpoint->tx_data_length = string_descriptors[index].length;
			}
		}
		else if (setup->wValue == (USB_DESCRIPTOR_TYPE_DEVICE << 8))
		{
			endpoint->tx_data = (uint8_t *)&device_descriptor;
			endpoint->tx_data_length = sizeof(device_descriptor);
		}
		else if (setup->wValue == (USB_DESCRIPTOR_TYPE_CONFIGURATION << 8))
		{
			endpoint->tx_data = (uint8_t *)&configuration;
			endpoint->tx_data_length = sizeof(configuration);
		}
		else if (setup->wValue == (USB_DESCRIPTOR_TYPE_HID_REPORT << 8))
		{
			endpoint->tx_data = (uint8_t *)&report_descriptor;
			endpoint->tx_data_length = sizeof(report_descriptor);
		}
		else
		{
			stall = true;
		}
		break;
	default:
		stall = true;
		break;
	}
	return stall;
}

void endpoint_prepare_next_tx(uint8_t endpoint)
{
	uint32_t tx_length = 0;
	endpoint_t *ep = endpoints[endpoint];
	buffer_descriptor_t *bd = &buffer_descriptor_table[BUFFER_DESCRIPTOR_INDEX(endpoint, TX, ep->buffer_in_use)];

	tx_length = ep->tx_data_length;
	if (tx_length > ep->bufferSize)
	{
		tx_length = ep->bufferSize;
	}

	bd->bufferAddress = ep->tx_data;
	bd->fields = BUFFER_DESCRIPTOR(tx_length, ep->data);

	ep->tx_data += tx_length;
	ep->tx_data_length -= tx_length;

	ep->buffer_in_use ^= 1;
	ep->data ^= 1;
}

void buffer_descriptor_rx_release(buffer_descriptor_t *buffer_descriptor, uint8_t bufferSize)
{
	uint8_t data = (buffer_descriptor->fields & BUFFER_DESCRIPTOR_DATA_MASK) ? 1 : 0;
	buffer_descriptor->fields = BUFFER_DESCRIPTOR(bufferSize, data);
}
