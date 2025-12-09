#include <stdio.h>
#include <stdlib.h>
unsigned char* dma_ptr; //
unsigned char* dma_ptr_old; //
unsigned char* transport_ptr; //
unsigned char* transport_ptr_old; //
unsigned char voice_buffer[32];
const int     CIRC_BUF_SIZE = 32;
unsigned char* CIRC_BUF_START = &voice_buffer[0]; //  margin-start
unsigned char* CIRC_BUF_END = &voice_buffer[31]; //  margin-end
unsigned short cndtr;
int  transferSize;

/*
START INITIALIZATION:
 0)Init  transferSize
 1)load CNDTR
 2)Init transport_ptr at  CIRC_BUF_START + (CIRC_BUF_SIZE / 2)
 4)Init  transport_ptr_old at (transport_ptr - transferSize)
 5)Init dma_ptr at  CIRC_BUF_START
 6)Init dma_ptr_old at (dma_ptr- transferSize)
*/

/*
NEW INCOMING DATA:
 Checking : are dma_pointer or transport_ptr be acheived of array`s margin?
  When yes: wrap it around

*/

int main()
{
    printf("Hello world!\n");
    return 0;
}
