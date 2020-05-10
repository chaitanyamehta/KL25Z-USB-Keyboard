#ifndef __USB_HID_SPEC_H
#define __USB_HID_SPEC_H

#include <stdint.h>

/* 
Class Descriptor Types 
Device Class Definition for HID Version 1.11 Page 49
*/
#define USB_DESCRIPTOR_TYPE_HID (0x21U)
#define USB_DESCRIPTOR_TYPE_HID_REPORT (0x22U)
#define USB_DESCRIPTOR_TYPE_HID_PHYSICAL (0x23U)

/* Device Class Definition for HID Version 1.11 Page 22 */
typedef __packed struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bChildDescriptorType;
    uint16_t wDescriptorLength;
} usb_hid_descriptor_t;

#endif
