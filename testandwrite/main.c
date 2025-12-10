#include <stdio.h>
#include <stdlib.h>
#include "string.h"
unsigned char voice_buffer[32];

typedef struct {
int dma_processed;
int prev_transport_processed;
unsigned char* dma_ptr; //
unsigned char* dma_ptr_old; //
unsigned char* transport_ptr; //
unsigned char* transport_ptr_old; //
int  CIRC_BUF_SIZE;
unsigned char* CIRC_BUF_START;
unsigned char* CIRC_BUF_END;
unsigned short cndtr;
int actual_difference;
int  transferSize;
} stream_h;

stream_h header_stream;
#define DMA_WIDH 1
/*
This function, I suppose, automatically sends "0" to a host in case of START (the
host begin data thansfer, a device has gone to active mode and it is the first transfer
*/
int onArrive(int usb_parsel_size /*in bytes*/, unsigned char* incomingData) {
      int sliceSize;
      int parselSize = usb_parsel_size;

  // 1)Has DMA jumped to a new transaction?
  //a)current value of the DMA pointer:
      header_stream.dma_ptr = ((header_stream.transferSize -  header_stream.cndtr) * DMA_WIDH) +  header_stream.CIRC_BUF_START;
  //b)Was there wrapping?
  if ( header_stream.dma_ptr <  header_stream.dma_ptr_old) {
      //when wrapping was occured, calculate processed data (last BEFORE end and FIRST after BEGIN of the circ buffer):
       header_stream.dma_processed = ( header_stream.CIRC_BUF_END -  header_stream.dma_ptr_old) + ( header_stream.dma_ptr -  header_stream.CIRC_BUF_START);
  }   else {//when a wrap has not been occured - subrtact  dma_ptr from dma_ptr_old
        header_stream.dma_processed =  header_stream.dma_ptr -  header_stream.dma_ptr_old;
  }
  ///--last DMA processed size has been calculated
  //c)update  dma_ptr_old
        header_stream.dma_ptr_old =  header_stream.dma_ptr;
  ///-------------------------------------
  //2)Will  transport_ptr be wraped after new USB data transfer?
  if (( header_stream.transport_ptr + parselSize) >  header_stream.CIRC_BUF_END) {
    //when a wrap will ouccured - write data by two steps:
    //a)Write the first part - write until buffer`s END :
    sliceSize =  header_stream.transport_ptr -  header_stream.CIRC_BUF_END;
    memcpy( header_stream.transport_ptr, incomingData,sliceSize);
    usb_parsel_size = parselSize- sliceSize; //update amount of data - (because a first part has been written)
    incomingData = incomingData + sliceSize; //move a pointer (because a first part has been written)
     header_stream.transport_ptr =  header_stream.CIRC_BUF_START; //update the pointer, it must point to the begin of the circular DMA buffer
    //b)Write the second part - from BEGIN of circular DMA buffer to achieve full incoming USB data amount
    memcpy( header_stream.transport_ptr, incomingData, parselSize);
  } else {
   //when there was no wrapping - move data easily :
    memcpy( header_stream.transport_ptr, incomingData, parselSize);
  }
  //3) Calculate difference between DMA consumed data amount (now) and USB transfered data amount (previous)
    header_stream.actual_difference =   header_stream.dma_processed -  header_stream.prev_transport_processed;
   //4) Update prev_transport_processed for next new USB data event
    header_stream.prev_transport_processed = usb_parsel_size;
   return  header_stream.actual_difference;

}

/*
When a new USB data has arrived:
  1)Has DMA jumped to a new transaction?
     When yes, calculate data was written since last USB transfer my method 2.
     Wnen NO, calculate data was written by easyst way - method1

  3)How many data was written in previous DMA transaction?
  4)How many data was consumed by a DMA since the last USB transaction?
  5)Calculate difference DMA_consumed_since_last_USB_transfer - last_USB_transfer_size
     and send to a host.
  6)Will a transport_ptr be wrapped ,after write a new USB transfer?
    When yes, write first (before the circ array end) and
      the second part (after begin of the circ array),calculate and assign a new tranport_pointer value.
     When not - update the pointer.

*/



int main() {

   header_stream.CIRC_BUF_START = &voice_buffer[0]; //  margin-start
   header_stream.CIRC_BUF_END = &voice_buffer[31]; //  margin-end
   header_stream.CIRC_BUF_SIZE = 32;
   header_stream.transferSize = 8;
   //load some data into array
   strcpy (voice_buffer,".00102030405060708090A0B0C0D0E0F10");
   header_stream.cndtr = 32;
   header_stream.dma_ptr = header_stream.CIRC_BUF_START;
   header_stream.dma_ptr_old = header_stream.CIRC_BUF_END - header_stream.transferSize;
   header_stream.transport_ptr =  header_stream.CIRC_BUF_START + (header_stream.transferSize / 2);
   header_stream.transport_ptr_old = header_stream.CIRC_BUF_END -  header_stream.transferSize;
   onArrive(4,"XXXX");


    printf("Hello world!\n");
    return 0;
}
