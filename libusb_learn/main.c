#include <stdio.h>
#include <stdlib.h>
#include "libusb.h"
#include "libusb_dyn.h"

#define VID 0xCafe
#define PID 0x4000

#define EP_OUT 0x01   // endpoint 1 OUT
#define EP_IN  0x81   // endpoint 1 IN

libusb_context *ctx = NULL;
libusb_device_handle *dev = NULL;

int main()
{

    if (!load_libusb())
        return 1;

    if (p_libusb_init(&ctx) < 0) {
        printf("libusb_init failed\n");
        return 1;
    }

    dev = p_libusb_open_device_with_vid_pid(ctx, VID, PID);
    if (!dev) {
        printf("Device not found\n");
        return 1;
    }

    if (p_libusb_claim_interface(dev, 0) < 0) {
        printf("Cannot claim interface 0\n");
        return 1;
    }

    // --- Send data ---
    unsigned char tx[4] = {1, 2, 3, 4};
    int written = 0;
    int r = p_libusb_bulk_transfer(dev, EP_OUT, tx, sizeof(tx), &written, 1000);

    printf("Write result = %d, bytes = %d\n", r, written);

    // --- Receive data ---
    unsigned char rx[64];
    int read = 0;
    r = p_libusb_bulk_transfer(dev, EP_IN, rx, sizeof(rx), &read, 1000);

    printf("Read result = %d, bytes = %d\n", r, read);

    // Print received bytes
    for (int i=0; i<read; i++)
        printf("%02X ", rx[i]);
    printf("\n");

    // Cleanup
    p_libusb_release_interface(dev, 0);
    p_libusb_close(dev);
    p_libusb_exit(ctx);

    return 0;
}
