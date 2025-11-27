#ifndef LIBUSB_DYN_H
#define LIBUSB_DYN_H

#include <windows.h>
#include "libusb.h"

// ----------------------------
// Function pointer typedefs
// ----------------------------

// Init / Exit
typedef int  (*libusb_init_t)(libusb_context **ctx);
typedef void (*libusb_exit_t)(libusb_context *ctx);

// Device list / enumeration
typedef ssize_t (*libusb_get_device_list_t)
    (libusb_context *ctx, libusb_device ***list);

typedef void (*libusb_free_device_list_t)
    (libusb_device **list, int unref_devices);

typedef int (*libusb_get_device_descriptor_t)
    (libusb_device *dev, struct libusb_device_descriptor *desc);

typedef int (*libusb_get_config_descriptor_t)
    (libusb_device *dev, uint8_t config_index,
     struct libusb_config_descriptor **config);

typedef void (*libusb_free_config_descriptor_t)
    (struct libusb_config_descriptor *config);

// Device open / close
typedef libusb_device_handle * (*libusb_open_device_with_vid_pid_t)
    (libusb_context *ctx, uint16_t vendor, uint16_t product);

typedef int (*libusb_open_t)
    (libusb_device *dev, libusb_device_handle **handle);

typedef void (*libusb_close_t)
    (libusb_device_handle *dev);

// Interfaces
typedef int (*libusb_claim_interface_t)
    (libusb_device_handle *dev, int interface_number);

typedef int (*libusb_release_interface_t)
    (libusb_device_handle *dev, int interface_number);

typedef int (*libusb_set_configuration_t)
    (libusb_device_handle *dev, int configuration);

// Transfers: BULK, INT, CONTROL
typedef int (*libusb_bulk_transfer_t)
    (libusb_device_handle *dev, unsigned char ep,
     unsigned char *data, int len, int *transferred,
     unsigned int timeout);

typedef int (*libusb_interrupt_transfer_t)
    (libusb_device_handle *dev, unsigned char ep,
     unsigned char *data, int len, int *transferred,
     unsigned int timeout);

typedef int (*libusb_control_transfer_t)
    (libusb_device_handle *dev, uint8_t bmRequestType,
     uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
     unsigned char *data, uint16_t wLength,
     unsigned int timeout);

// Isochronous transfers (ASYNC)
typedef struct libusb_transfer * (*libusb_alloc_transfer_t)
    (int iso_packets);

typedef void (*libusb_free_transfer_t)
    (struct libusb_transfer *transfer);

typedef int (*libusb_submit_transfer_t)
    (struct libusb_transfer *transfer);

typedef int (*libusb_cancel_transfer_t)
    (struct libusb_transfer *transfer);

typedef void (*libusb_set_iso_packet_lengths_t)
    (struct libusb_transfer *transfer, unsigned int length);

// Sync API for completed async transfers
typedef int (*libusb_handle_events_t)
    (libusb_context *ctx);

// Kernel driver
typedef int (*libusb_kernel_driver_active_t)
    (libusb_device_handle *dev, int interface_number);

typedef int (*libusb_detach_kernel_driver_t)
    (libusb_device_handle *dev, int interface_number);

typedef int (*libusb_attach_kernel_driver_t)
    (libusb_device_handle *dev, int interface_number);

// Errors
typedef const char * (*libusb_error_name_t)(int code);
typedef const char * (*libusb_strerror_t)(enum libusb_error code);

// Reset device
typedef int (*libusb_reset_device_t)
    (libusb_device_handle *dev);

// --------------------------------
// EXTERN DECLARATIONS (GLOBAL FP)
// --------------------------------

extern libusb_init_t                      p_libusb_init;
extern libusb_exit_t                      p_libusb_exit;

extern libusb_get_device_list_t           p_libusb_get_device_list;
extern libusb_free_device_list_t          p_libusb_free_device_list;

extern libusb_get_device_descriptor_t     p_libusb_get_device_descriptor;
extern libusb_get_config_descriptor_t     p_libusb_get_config_descriptor;
extern libusb_free_config_descriptor_t    p_libusb_free_config_descriptor;

extern libusb_open_device_with_vid_pid_t  p_libusb_open_device_with_vid_pid;
extern libusb_open_t                      p_libusb_open;
extern libusb_close_t                     p_libusb_close;

extern libusb_claim_interface_t           p_libusb_claim_interface;
extern libusb_release_interface_t         p_libusb_release_interface;

extern libusb_bulk_transfer_t             p_libusb_bulk_transfer;
extern libusb_interrupt_transfer_t        p_libusb_interrupt_transfer;
extern libusb_control_transfer_t          p_libusb_control_transfer;

extern libusb_alloc_transfer_t            p_libusb_alloc_transfer;
extern libusb_free_transfer_t             p_libusb_free_transfer;
extern libusb_submit_transfer_t           p_libusb_submit_transfer;
extern libusb_cancel_transfer_t           p_libusb_cancel_transfer;
extern libusb_set_iso_packet_lengths_t    p_libusb_set_iso_packet_lengths;
extern libusb_handle_events_t             p_libusb_handle_events;

extern libusb_kernel_driver_active_t      p_libusb_kernel_driver_active;
extern libusb_detach_kernel_driver_t      p_libusb_detach_kernel_driver;
extern libusb_attach_kernel_driver_t      p_libusb_attach_kernel_driver;

extern libusb_error_name_t                p_libusb_error_name;
extern libusb_strerror_t                  p_libusb_strerror;

extern libusb_reset_device_t              p_libusb_reset_device;

// Loader
int load_libusb(void);
void unload_libusb(void);

#endif
