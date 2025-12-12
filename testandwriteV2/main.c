#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 unsigned char dma_buff[32];
 int backPressure = 0;
 int parselLength = 10;
 unsigned char sampleData[]="<---ABCDEFGH--->";

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

stream_head __attribute__((aligned(64))) hOfStream; //must be aligned to 32 in STM32 (because a CPU core is 32bit)


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
     if( ((usbIncomLen-1) + hOfStream.transportPtrCurr) > hOfStream.CIRC_ARR_END ) {
          //a) write the first part  until the END buffer`s cell
         var1 = (hOfStream.CIRC_ARR_END - hOfStream.transportPtrCurr) + 1; //data amount until buffer_end
         memcpy(hOfStream.transportPtrCurr, newUsbParcelPtr, var1);      //copy
         newUsbParcelPtr += var1;    //update a pointer
         usbIncomLen -= var1;  //correct data about subtract written value
          //b) write the second part from START until all data was written
         hOfStream.transportPtrCurr = hOfStream.CIRC_ARR_START;
         memcpy(hOfStream.transportPtrCurr, newUsbParcelPtr,  usbIncomLen);
         hOfStream.transportPtrCurr += usbIncomLen; //update a pointer
     } else {
      //when wrap will not happened - write a new data by the easist way
         memcpy(hOfStream.transportPtrCurr, newUsbParcelPtr,  usbIncomLen);
         hOfStream.transportPtrCurr += usbIncomLen;//update a pointer
     }
     //when a pointer is great that array`s MAX element, assign first:
     if(hOfStream.transportPtrCurr > hOfStream.CIRC_ARR_END){
        hOfStream.transportPtrCurr = hOfStream.CIRC_ARR_START;
     }
     //3)Calculate a correction for a host
      hostCorrection =  dmaConsumed - usbParcelLengh;

     //5)Return a correction for the host and real time flow control:
     //When DMA consumed less that previous USB transfer, then the result < 0
     //When DMA consumed greater that previous USB transfer, then the result is > 0
      printf("DMA consumed: %d, USB written: %d, correction: %d \n",dmaConsumed,usbParcelLengh,hostCorrection);
      return hostCorrection;

}





int main()
{
    //emulate init process
    hOfStream.CIRC_ARR_START = &dma_buff[0];
    hOfStream.CIRC_ARR_END = &dma_buff[31];
    hOfStream.DMA_BUFFER_STEPS = 32;
    hOfStream.DMA_BUS_WIDTH = 1;//1 byte

    hOfStream.dmaReminderCurr = 32;
    hOfStream.dmaReminderPrev = 32;
    hOfStream.transportPtrCurr = hOfStream.CIRC_ARR_START + ((hOfStream.DMA_BUFFER_STEPS / 2) * hOfStream.DMA_BUS_WIDTH);
    strcpy(dma_buff,".102030405060708090a0b0c0d0e0f10");
    //START state - now the DMA at the begin of the buffer, the transport pointer (for write new data) is in the middle of the buffer
    //1)now write the first USB packet - it consists 10 bytes, so it is fast that DMA can consumed (8). DMA has not consumed nothing yet
      backPressure = onNewUsbDataCb("HELLO_WORD!", parselLength);
    //2)DMA has consumed only 8 bytes.The device received also the same USB data amount but gives a backpressure:
      hOfStream.dmaReminderCurr -= 7;
      parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb("HAPPY_NEW_YEAR!", parselLength);
    //3)Now we have corrected packet size
    hOfStream.dmaReminderCurr -= 7;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb("IT_IS_RAINING!",parselLength);
     //4)Runs next
    hOfStream.dmaReminderCurr -= 7;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">BILL_GATES!",parselLength);
    //5)Runs next
    hOfStream.dmaReminderCurr -= 7;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">BILL_GATES!",parselLength);
     //6)The wrap will happened (5*7=35, so CNDTR becomes DMA_length - (35 -DMA_lngth)=29

         hOfStream.dmaReminderCurr = 29;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

         hOfStream.dmaReminderCurr = 22;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

         hOfStream.dmaReminderCurr = 13;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

         hOfStream.dmaReminderCurr = 8;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

         hOfStream.dmaReminderCurr = 1;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

         hOfStream.dmaReminderCurr = 26;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

         hOfStream.dmaReminderCurr = 22;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

               hOfStream.dmaReminderCurr = 18;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

            hOfStream.dmaReminderCurr = 11;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

                  hOfStream.dmaReminderCurr = 4;
    parselLength += backPressure; //correct a lengh of a parsel to preventing over/underrun
      backPressure = onNewUsbDataCb(">ILON_MUSK!",parselLength);

    return 0;
}
