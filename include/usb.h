#ifndef __USB_H
#define __USB_H

#include <stdint.h>
#include "buffer_descriptor.h"

#define PID_OUT 0x1
#define PID_IN 0x9
#define PID_SOF 0x5
#define PID_SETUP 0xd

#define ENDPOINT0_SIZE 64
#define ENDPOINT1_SIZE 8

#define NUM_ENDPOINTS 2
#define NUM_BUFFER_DESCRIPTORS (NUM_ENDPOINTS * 4)

typedef struct
{
	uint8_t bufferSize;
  uint8_t *rx_even;
  uint8_t *rx_odd;

  uint8_t buffer_in_use;
  uint8_t data;

  uint8_t *tx_data;
  uint16_t tx_data_length;

  void (*handler)(uint8_t endpoint, uint8_t token_pid, buffer_descriptor_t *buffer_descriptor);
} endpoint_t;

void USB_Init(void);
void USB_SetEndpoint(uint8_t index, endpoint_t *endpoint);
endpoint_t *USB_GetEndpoint(uint8_t index);
void endpoint_prepare_next_tx(uint8_t endpoint);

#endif
