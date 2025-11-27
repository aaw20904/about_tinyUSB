#include "libusb_dyn.h"
#include <stdio.h>

HMODULE hLib = NULL;

// Macro to load functions
#define LOAD(func, type)                                     \
    p_##func = (type)GetProcAddress(hLib, #func);            \
    if (!p_##func) {                                         \
        printf("Missing function in DLL: %s\n", #func);      \
        return 0;                                            \
    }

// ---- Global function pointers ----

libusb_init_t                      p_libusb_init;
libusb_exit_t                      p_libusb_exit;

libusb_get_device_list_t           p_libusb_get_device_list;
libusb_free_device_list_t          p_libusb_free_device_list;

libusb_get_device_descriptor_t     p_libusb_get_device_descriptor;
libusb_get_config_descriptor_t     p_libusb_get_config_descriptor;
libusb_free_config_descriptor_t    p_libusb_free_config_descriptor;

libusb_open_device_with_vid_pid_t  p_libusb_open_device_with_vid_pid;
libusb_open_t                      p_libusb_open;
libusb_close_t                     p_libusb_close;

libusb_claim_interface_t           p_libusb_claim_interface;
libusb_release_interface_t         p_libusb_release_interface;

libusb_bulk_transfer_t             p_libusb_bulk_transfer;
libusb_interrupt_transfer_t        p_libusb_interrupt_transfer;
libusb_control_transfer_t          p_libusb_control_transfer;

libusb_alloc_transfer_t            p_libusb_alloc_transfer;
libusb_free_transfer_t             p_libusb_free_transfer;
libusb_submit_transfer_t           p_libusb_submit_transfer;
libusb_cancel_transfer_t           p_libusb_cancel_transfer;
libusb_set_iso_packet_lengths_t    p_libusb_set_iso_packet_lengths;
libusb_handle_events_t             p_libusb_handle_events;

libusb_kernel_driver_active_t      p_libusb_kernel_driver_active;
libusb_detach_kernel_driver_t      p_libusb_detach_kernel_driver;
libusb_attach_kernel_driver_t      p_libusb_attach_kernel_driver;

libusb_error_name_t                p_libusb_error_name;
libusb_strerror_t                  p_libusb_strerror;

libusb_reset_device_t              p_libusb_reset_device;

// ---- Loader ----

int load_libusb(void)
{
    hLib = LoadLibraryA("msys-usb-1.0.dll");
    if (!hLib) {
        printf("Failed to load msys-usb-1.0.dll\n");
        return 0;
    }

    LOAD(libusb_init,                    libusb_init_t);
    LOAD(libusb_exit,                    libusb_exit_t);

    LOAD(libusb_get_device_list,         libusb_get_device_list_t);
    LOAD(libusb_free_device_list,        libusb_free_device_list_t);

    LOAD(libusb_get_device_descriptor,   libusb_get_device_descriptor_t);
    LOAD(libusb_get_config_descriptor,   libusb_get_config_descriptor_t);
    LOAD(libusb_free_config_descriptor,  libusb_free_config_descriptor_t);

    LOAD(libusb_open_device_with_vid_pid,libusb_open_device_with_vid_pid_t);
    LOAD(libusb_open,                    libusb_open_t);
    LOAD(libusb_close,                   libusb_close_t);

    LOAD(libusb_claim_interface,         libusb_claim_interface_t);
    LOAD(libusb_release_interface,       libusb_release_interface_t);

    LOAD(libusb_bulk_transfer,           libusb_bulk_transfer_t);
    LOAD(libusb_interrupt_transfer,      libusb_interrupt_transfer_t);
    LOAD(libusb_control_transfer,        libusb_control_transfer_t);

    LOAD(libusb_alloc_transfer,          libusb_alloc_transfer_t);
    LOAD(libusb_free_transfer,           libusb_free_transfer_t);
    LOAD(libusb_submit_transfer,         libusb_submit_transfer_t);
    LOAD(libusb_cancel_transfer,         libusb_cancel_transfer_t);
   // LOAD(libusb_set_iso_packet_lengths,  libusb_set_iso_packet_lengths_t);
    LOAD(libusb_handle_events,           libusb_handle_events_t);

    LOAD(libusb_kernel_driver_active,    libusb_kernel_driver_active_t);
    LOAD(libusb_detach_kernel_driver,    libusb_detach_kernel_driver_t);
    LOAD(libusb_attach_kernel_driver,    libusb_attach_kernel_driver_t);

    LOAD(libusb_error_name,              libusb_error_name_t);
    LOAD(libusb_strerror,                libusb_strerror_t);

   // LOAD(libusb_reset_device,            libusb_reset_device_t);

    return 1;
}

void unload_libusb(void)
{
    if (hLib) {
        FreeLibrary(hLib);
        hLib = NULL;
    }
}
