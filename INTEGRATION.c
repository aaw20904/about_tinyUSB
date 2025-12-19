/*To integrate  the TinyUSB into a STM32CubeIDE project:
 1)Create a folder "libraries"
----------------------------------***----------------------------------------
 2)Create inner folders "tinyusb/src/" 
   --------------------------------***--------------------------------------
 3)Copy inside src/ the next folders: class, common, device, osal, portable
   and files tusb.c, tusb.h, tusb_option.h.
   //NOTE: copy portable/ST for STM32F103, and "portable/synopsys" for STM32F405
     ------------------------------***-----------------------------------------
 4)Right click on "libraries" folder, properties->C/C++BuildSettings.There are a flag 
  "Exclude resource from build", uncheck it to enable compile this folder.
    --------------------------------***---------------------------------------
 5)Stay the folder "portable/st/stm32_fsdev/" 
   ----------------------------------***-------------------------------------
 6) Add include pats inside compiler optoins:
   Project → Properties → C/C++ General → paths And Symbols → GNU C
   Push "Add" and insert consequently the next strings:
    libraries/
    libraries/TinyUSB
    libraries/TinyUSB/src
	libraries/TinyUSB/src/common
	libraries/TinyUSB/src/device
	libraries/TinyUSB/src/class
	libraries/tinyusb/src/osal
	libraries/TinyUSB/src/portable/st/stm32_fsdev
	//NOTE1: FOR STM32F405 exclude  libraries/TinyUSB/src/portable/st/stm32_fsdev
     and add libraries/tinyusb/src/portable/synopsys/dwc2 instead 
	NOTE2: Add neccessary in both "Assembly" and "GNU C" (these options are next each to other)
	  --------------------------------***---------------------------------------
 7)Create in "Core/Inc/" confiuration file "tusb_config.h":
*/
#pragma once

#define CFG_TUSB_MCU                OPT_MCU_STM32F1
#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)

#define CFG_TUD_ENDPOINT0_SIZE      64

#define CFG_TUD_VENDOR              1
#define CFG_TUD_VENDOR_RX_BUFSIZE   64
#define CFG_TUD_VENDOR_TX_BUFSIZE   64
/*
7)Enable USB only ( HAL middleware must be turned OFF),
  enable interrupt handlers for USB, 
  re-build the code.
  -------------------------------***----------------------------------------
8)Modify interrupt handler, include "tusb.h" in ISR file, 
   remove inner HAL function-handler (stay only interrupt procedures),
   insert tinyusb's   function  "tud_int_handler()":
*/
void USB_HP_CAN1_TX_IRQHandler(void)
{
  /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 0 */
                 tud_int_handler(0);   // call TinyUSB ISR
  /* USER CODE END USB_HP_CAN1_TX_IRQn 0 */
   ///////HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 1 */

  /* USER CODE END USB_HP_CAN1_TX_IRQn 1 */
}

/**
  * @brief This function handles USB low priority or CAN RX0 interrupts.
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 0 */
                 tud_int_handler(0);   // call TinyUSB ISR
  /* USER CODE END USB_LP_CAN1_RX0_IRQn 0 */
     //////HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 1 */

  /* USER CODE END USB_LP_CAN1_RX0_IRQn 1 */
}
  tud_int_handler(0);   // call TinyUSB ISR
/*--------------------------------------***--------------------------------------------------
9)Initialize TinyUSB in main():
*/
#include "tusb.h"

tusb_init();         // TinyUSB init, insert it after clock ready and HAL initialization

tud_task();          // TinyUSB background task, insert it insideinfinite loop or the SysTick handler.
                      //It must be called periodically 
 /*10)Create TinyUSB initialization file "Core/Src/usb_descriptors.c" 
There are all the descriptors and configuraion callbacks.
Here the example of vendor 
 device with raw descriptors, it is avaliable and convenient to use macroses 
 instead of raw descriptors */ 
 
 #include "tusb.h"

/*------------------------------------------------------------------*/
/* Device descriptor */
/*------------------------------------------------------------------*/

#define USB_VID   0xCafe
#define USB_PID   0x4000
#define USB_BCD   0x0100

//USER custom definition - to have explicit descriptors insted of embedded macroses:

 

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
    tusb_desc_endpoint_t       ep_out_1;
    tusb_desc_endpoint_t        ep_in_1;
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
        .bLength          = sizeof(tusb_desc_endpoint_t),
    	    .bDescriptorType  = TUSB_DESC_ENDPOINT,
    	    .bEndpointAddress = 0x01,               // EP1 OUT
    	    .bmAttributes = {
    	        .xfer  = TUSB_XFER_BULK,
    	        .sync  = 0,
    	        .usage = 0,
    	    },
    	    .wMaxPacketSize   = 64,
    	    .bInterval        = 0
    },

    .ep_in_1 = {
       .bLength          = sizeof(tusb_desc_endpoint_t),
    		    .bDescriptorType  = TUSB_DESC_ENDPOINT,
    		    .bEndpointAddress = 0x81,               // EP1 IN
    		    .bmAttributes = {
    		        .xfer  = TUSB_XFER_BULK,
    		        .sync  = 0,
    		        .usage = 0,
    		    },
    		    .wMaxPacketSize   = 64,
    		    .bInterval        = 0
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
/*-------------------------------***--------------------------------------
10) Disable TinyUSB OS abstraction (osal) if you are not use operating system:
  in the file "tusb_config.h" 
*/

#define CFG_TUSB_OS   OPT_OS_NONE

//This fixes include error:  'fatal error: osal/osal.h not found!'
/*--------------------------------***------------------------------------------
11)Implement callbacks inside  usb_descriptors.c OR in main.c (your choice):
*/
void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint16_t bufsize)
{
    uint8_t buf[64];
    uint32_t count = tud_vendor_read(buf, bufsize);
    // do something with received bytes
}

void tud_vendor_tx_cb(uint8_t itf, uint32_t sent_bytes)
{
    // TX finished
	 GPIOC->BSRR = GPIO_BSRR_BS13;
}














