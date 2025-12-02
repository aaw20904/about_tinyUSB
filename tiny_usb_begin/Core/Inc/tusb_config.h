/*
 * tusb_config.h
 *
 *  Created on: Nov 26, 2025
 *      Author: Andrew
 */

#ifndef TINYUSB_TUSB_CONFIG_H_
#define TINYUSB_TUSB_CONFIG_H_

#pragma once

// adjust to your MCU family:
#define CFG_TUSB_MCU             OPT_MCU_STM32F1 // OPT_MCU_STM32F4   // / STM32L4 / STM32F7 etc

#define CFG_TUSB_RHPORT0_MODE     (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#define CFG_TUD_ENDPOINT0_SIZE    64

// enable only vendor class
#define CFG_TUD_CDC               0
#define CFG_TUD_HID               0
#define CFG_TUD_MSC               0
#define CFG_TUD_VENDOR            1

#define CFG_TUD_VENDOR_RX_BUFSIZE 64
#define CFG_TUD_VENDOR_TX_BUFSIZE 64

#endif /* TINYUSB_TUSB_CONFIG_H_ */
