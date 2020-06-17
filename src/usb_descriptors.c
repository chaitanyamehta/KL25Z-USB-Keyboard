#include <stdint.h>
#include "usb.h"
#include "usb_spec.h"
#include "usb_hid_spec.h"
#include "usb_descriptors.h"

#define CHAR_TO_UTF16(x) (x),0x00

const uint8_t language_descriptor[] = {
    4,                          //blength
    USB_DESCRIPTOR_TYPE_STRING, //bDescriptorType
    0x09,                       //wLANGID[0] Lang ID for 0x0409 US English
    0x04};

const uint8_t manufacturer_string_descriptor[] = {
    14,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('c'),
    CHAR_TO_UTF16('m'),
    CHAR_TO_UTF16('e'),
    CHAR_TO_UTF16('h'),
    CHAR_TO_UTF16('t'),
    CHAR_TO_UTF16('a')};

const uint8_t product_string_descriptor[] = {
    18,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('k'),
    CHAR_TO_UTF16('e'),
    CHAR_TO_UTF16('y'),
    CHAR_TO_UTF16('b'),
    CHAR_TO_UTF16('o'),
    CHAR_TO_UTF16('a'),
    CHAR_TO_UTF16('r'),
    CHAR_TO_UTF16('d')};

const uint8_t serial_string_descriptor[] = {
    18,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('0'), 
    CHAR_TO_UTF16('0'), 
    CHAR_TO_UTF16('0'), 
    CHAR_TO_UTF16('0'), 
    CHAR_TO_UTF16('0'), 
    CHAR_TO_UTF16('0'), 
    CHAR_TO_UTF16('1')};

const uint8_t interface_string_descriptor[] = {
    8,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('h'),
    CHAR_TO_UTF16('i'),
    CHAR_TO_UTF16('d')};

const uint8_t configuration_string_descriptor[] = {
    16,
    USB_DESCRIPTOR_TYPE_STRING,
    CHAR_TO_UTF16('d'),
    CHAR_TO_UTF16('e'),
    CHAR_TO_UTF16('f'),
    CHAR_TO_UTF16('a'),
    CHAR_TO_UTF16('u'),
    CHAR_TO_UTF16('l'),
    CHAR_TO_UTF16('t')};

const usb_device_descriptor_t device_descriptor = {
    .bLength = 18,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = 0x0101,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = ENDPOINT0_SIZE,
    .idVendor = 0xFFFF,
    .idProduct = 0x0001,
    .bcdDevice = 0x0100,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1};

const uint8_t report_descriptor[REPORT_DESCRIPTORS] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};



const usb_configuration_t configuration = {
    .configuration_descriptor = {
        .bLength = 9,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,
        .wTotalLength = sizeof(usb_configuration_t),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 5,
        .bmAttributes = 0xA0,
        .bMaxPower = 0xFA
    },
    .interface_descriptor = {
        .bLength = 9,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 1,
        // HID
        .bInterfaceClass = 3,
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,
        .iInterface = 4
    },
    .hid_descriptor = {
        .bLength = 9,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_HID,
        .bcdHID = 0x0101,
        .bCountryCode = 0,
        .bNumDescriptors = 1,
        // Report
        .bChildDescriptorType = USB_DESCRIPTOR_TYPE_HID_REPORT,
        .wDescriptorLength = sizeof(report_descriptor)
    },
    .endpoint_descriptor = {
        .bLength = 7,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
        // Endpoint 1 - In
        .bEndpointAddress = 0x81,
        // Interrupt
        .bmAttributes = 0x03,
        .wMaxPacketSize = ENDPOINT1_SIZE,
        .bInterval = 0x0A,
    }};

const descriptor_entry_t string_descriptors[STRING_DESCRIPTORS] = {
    {(void *)&language_descriptor, sizeof(language_descriptor)},
    {(void *)&manufacturer_string_descriptor, sizeof(manufacturer_string_descriptor)},
    {(void *)&product_string_descriptor, sizeof(product_string_descriptor)},
    {(void *)&serial_string_descriptor, sizeof(serial_string_descriptor)},
    {(void *)&interface_string_descriptor, sizeof(interface_string_descriptor)},
    {(void *)&configuration_string_descriptor, sizeof(configuration_string_descriptor)},
};
