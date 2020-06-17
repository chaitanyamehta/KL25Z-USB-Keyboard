#ifndef __USB_H
#define __USB_H

#include <stdint.h>
#include "buffer_descriptor.h"

// Token
#define PID_OUT       0b0001
#define PID_IN        0b1001
#define PID_SOF       0b0101
#define PID_SETUP     0b1101
// Data
#define PID_DATA0     0b0011
#define PID_DATA1     0b1011
#define PID_DATA2     0b0111
#define PID_MDATA     0b1111
// Handshake
#define PID_ACK       0b0010
#define PID_NACK      0b1010
#define PID_STALL     0b1110
#define PID_NYET      0b0110
// Special
#define PID_PRE       0b1100
#define PID_ERR       0b1100
#define PID_SPLIT     0b1000
#define PID_PING      0b0100
#define PID_RESERVED  0b0000

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
