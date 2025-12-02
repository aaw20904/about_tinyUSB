#include "tusb.h"

/*------------------------------------------------------------------*/
/* Device descriptor */
/*------------------------------------------------------------------*/

#define USB_VID   0xCafe
#define USB_PID   0x4000
#define USB_BCD   0x0100

//USER custom definition - to have explicit descriptors insted of embedded macroses:

//ENDPOINT descriptor
typedef struct __attribute__((packed))
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
} usb_endpoint_descriptor_t;

//INTERFACE descriptor
typedef struct __attribute__((packed))
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} usb_interface_descriptor_t;

//CONFIGURATION descriptor
typedef struct __attribute__((packed))
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
} usb_config_descriptor_t;

//full descriptor
typedef struct __attribute__((packed))
{
    usb_config_descriptor_t        config;
    usb_interface_descriptor_t     interface0;
    usb_endpoint_descriptor_t      ep_out_1;
    usb_endpoint_descriptor_t      ep_in_1;
} usb_full_config_t;

//The instance of full descriptor with inner descriptors and definitions
usb_full_config_t const desc_cfg =
{
    .config = {
        .bLength            = 9,
        .bDescriptorType    = TUSB_DESC_CONFIGURATION,
        .wTotalLength       = sizeof(usb_full_config_t),
        .bNumInterfaces     = 1,
        .bConfigurationValue= 1,
        .iConfiguration     = 0,
        .bmAttributes       = 0x80,
        .bMaxPower          = 50
    },

    .interface0 = {
        .bLength            = 9,
        .bDescriptorType    = TUSB_DESC_INTERFACE,
        .bInterfaceNumber   = 0,
        .bAlternateSetting  = 0,
        .bNumEndpoints      = 2,
        .bInterfaceClass    = 0xFF,   // Vendor
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x00,
        .iInterface         = 0
    },

    .ep_out_1 = {
        .bLength            = 7,
        .bDescriptorType    = TUSB_DESC_ENDPOINT,
        .bEndpointAddress   = 0x01,      // OUT endpoint 1
        .bmAttributes       = 0x02,      // BULK
        .wMaxPacketSize     = 64,
        .bInterval          = 0
    },

    .ep_in_1 = {
        .bLength            = 7,
        .bDescriptorType    = TUSB_DESC_ENDPOINT,
        .bEndpointAddress   = 0x81,      // IN endpoint 1
        .bmAttributes       = 0x02,      // BULK
        .wMaxPacketSize     = 64,
        .bInterval          = 0
    },
};


// Device Descriptor
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,

    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = USB_BCD,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};



// TinyUSB callback: return pointer to device descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
    return (uint8_t const *) &desc_device;
}

/*------------------------------------------------------------------*/
/* Configuration descriptor */
/*------------------------------------------------------------------*/

enum
{
    ITF_NUM_VENDOR = 0,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN   (TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN)
/*
uint8_t const desc_configuration[] =
{
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 50),

    // Interface 0: Vendor class
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 0, 0x01, 0x81, 64)
};
*/

/*
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
    (void) index;
    return desc_configuration;
}*/
// TinyUSB callback: return pointer to the configuration descriptor
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
    (void) index;
    return (uint8_t const *)&desc_cfg;
}


/*------------------------------------------------------------------*/
/* String Descriptors */
/*------------------------------------------------------------------*/

char const* string_desc_arr[] =
{
    (const char[]) { 0x09, 0x04 }, // 0: English (US)
    "MyVendor",                    // 1: Manufacturer
    "MyUSB Device",                // 2: Product
    "123456",                      // 3: Serial number
};

// TinyUSB callback: return string descriptor
uint16_t const * tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    static uint16_t desc_str[32];
    uint8_t chr_count;

    if (index == 0)
    {
        // returned language code
        desc_str[1] = (uint16_t) (string_desc_arr[0][1] << 8) | string_desc_arr[0][0];
        chr_count = 1;
    }
    else
    {
        const char* str = string_desc_arr[index];
        chr_count = strlen(str);

        // limit string to 31 chars
        if (chr_count > 31) chr_count = 31;

        for (uint8_t i = 0; i < chr_count; i++)
        {
            desc_str[1 + i] = str[i];
        }
    }

    // first byte
    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return desc_str;
}
