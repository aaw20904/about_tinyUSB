#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 unsigned char dma_buff[32];

typedef struct {
  int dmaReminderPrev;  //CNDTR when previous USB data been written (read)
  int dmaReminderCurr;  //CNDTR now
  unsigned char* transportPtrCurr; //a pointer where new USB data will be written(read)
  int prevUsbransferSize;
  int DMA_BUFFER_STEPS; //amounts of DMA transactions (CNDTR when DMA is starting the transaction)
  unsigned char* CIRC_ARR_START; //a pointer to the first DMA memory cell
  unsigned char* CIRC_ARR_END;//a pointer to the last DMa memory cell
  int DMA_BUS_WIDTH; //a width of a bus of the DMA channel (byte, word, dword)
}stream_head;

stream_head hOfStream;


int onNewUsbDataCb (unsigned char* newUsbParcelPtr, int usbParcelLengh ) {
    int var1;
    int usbIncomLen = usbParcelLengh; //in bytes
    int hostCorrection =0;
   int dmaConsumed; //in bytes
   int prevUsbPacketSize; //in bytes
   ///! HERE MUST be a procedure to read the 'live' CNDTR value from a MCU into the hOfStream.dmaReminderCurr
   //1)Was a DMA pointer been wrapped?
   if (hOfStream.dmaReminderCurr > hOfStream.dmaReminderPrev) {
     //when a DMA was gone through begin cell(CNDTR= DMA_BUFFER_STEPS means first cell, CNDTR=0 means last cell )
      dmaConsumed = hOfStream.dmaReminderPrev + (hOfStream.DMA_BUFFER_STEPS - hOfStream.dmaReminderCurr);
      dmaConsumed = dmaConsumed * hOfStream.DMA_BUS_WIDTH;
   } else {
     //when dma has not been wrapped - calculate by easyst way:
      dmaConsumed = hOfStream.dmaReminderPrev - hOfStream.dmaReminderCurr;
      dmaConsumed = dmaConsumed * hOfStream.DMA_BUS_WIDTH;
   }
     //2)Update dmaReminderPrev
     hOfStream.dmaReminderPrev = hOfStream.dmaReminderCurr;
     //3)will a  transportPtrCurr be wrapped when we write a new data?
     if( (usbIncomLen + hOfStream.transportPtrCurr) > hOfStream.CIRC_ARR_END ) {
          //a) write the first part  until the END buffer`s cell
         var1 = (hOfStream.CIRC_ARR_END - hOfStream.transportPtrCurr) + 1; //data amount until buffer_end
         memcpy(hOfStream.transportPtrCurr, newUsbParcelPtr, var1);      //copy
         newUsbParcelPtr += var1;    //update a pointer
         usbIncomLen -= var1;  //correct data about subtract written value
         hOfStream.transportPtrCurr += var1;
          //b) write the second part from START until all data was written
         hOfStream.transportPtrCurr = hOfStream.CIRC_ARR_START;
         memcpy(hOfStream.transportPtrCurr, newUsbParcelPtr,  usbIncomLen);
         hOfStream.transportPtrCurr += usbIncomLen; //update a pointer
     } else {
      //when wrap will not happened - write a new data by the easist way
         memcpy(hOfStream.transportPtrCurr, newUsbParcelPtr,  usbIncomLen);
         hOfStream.transportPtrCurr += usbIncomLen;//update a pointer
     }
     //3)Calculate a correction for a host: DMA_consumed - prevUsbransferSize
      hostCorrection =  dmaConsumed - hOfStream.prevUsbransferSize;
     //4)Update previous transfer size - for the next USB incoming data
      hOfStream.prevUsbransferSize = usbParcelLengh;
     //5)Return a correction for the host and real time flow control:
     //When DMA consumed less that previous USB transfer, then the result < 0
     //When DMA consumed greater that previous USB transfer, then the result is > 0
      return hostCorrection;

}





int main()
{
    //emulate init process
    hOfStream.CIRC_ARR_START = &dma_buff[0];
    hOfStream.CIRC_ARR_END = &dma_buff[31];
    hOfStream.DMA_BUFFER_STEPS = 32;
    hOfStream.DMA_BUS_WIDTH = 1;//1 byte
    hOfStream.prevUsbransferSize = 0; //data hasnt been transfered yet
    hOfStream.dmaReminderCurr = 32;
    hOfStream.dmaReminderPrev = 32;
    hOfStream.transportPtrCurr = hOfStream.CIRC_ARR_START + (hOfStream.DMA_BUFFER_STEPS * hOfStream.DMA_BUS_WIDTH);

    onNewUsbDataCb("example",7);

    printf("Hello world!\n");
    return 0;
}
