#ifndef __USB_DESCRIPTOR_H
#define __USB_DESCRIPTOR_H

#include <stdint.h>
#include "usb_spec.h"
#include "usb_hid_spec.h"

#define STRING_DESCRIPTORS 6
#define REPORT_DESCRIPTORS 63

typedef struct
{
    void *descriptor;
    uint16_t length;
} descriptor_entry_t;

typedef __packed struct
{
    usb_configuration_descriptor_t configuration_descriptor;
    usb_interface_descriptor_t interface_descriptor;
    usb_hid_descriptor_t hid_descriptor;
    usb_endpoint_descriptor_t endpoint_descriptor;
} usb_configuration_t;

extern const descriptor_entry_t string_descriptors[STRING_DESCRIPTORS];
extern const usb_device_descriptor_t device_descriptor;
extern const usb_configuration_t configuration;
extern const uint8_t report_descriptor[REPORT_DESCRIPTORS];

#endif
