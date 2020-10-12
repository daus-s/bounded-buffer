#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MMAP_SIZE 4096
#define BUFFER_SIZE 100
#define PAYLOAD_SIZE 34

item buffer[BUFFER_SIZE];
int ITEM_NO = 0;


typedef struct
{
     int item_no;                           /* number of the item produced */
     unsigned short cksum;                  /* 16-bit Internet checksum*/
     unsigned char payload[PAYLOAD_SIZE];   /* random generated data */
} item;


unsigned int ip_checksum(unsigned char *data, int nbytes)
{
    unsigned int sum = 0xffff;
    unsigned short word;

    int  i;

    // Handle complete 16-bit blocks.
    for (i = 0; i+1<nbytes; i+=2) {
        memcpy(&word, data+i, 2);
        sum += word;
        if (sum > 0xffff) {
            sum -= 0xffff;
        }
    }

    // Handle any partial block at the end of the data.
    if (nbytes&1) {
        word=0;
        memcpy(&word, data+nbytes-1, 1);
        sum += word;;
        if (sum > 0xffff) {
            sum -= 0xffff;
        }
    }

    // Return the checksum
    return ~sum;

}



/* consume the item in next_consumed */
/* 1. Check for no skipped buffers (item_no is contiguous) */
/* 2. Verify the calculated checksum matches what is stored in
next_consumed */
void next_consumed(item* ptr)
{
    ptr->
}

void consume()
{
    int in = 0;
    int out = 0;

    item next_consumed;

    while (true)
    {
        while (in == out)
        {
            sleep(1); /* do nothing but sleep for 1 second */
        }
        next_consumed = buffer[out];
        out = (out + 1) % BUFFER SIZE;


      }

}


int main(int argc, char const **argv)
{
    char* name = argv[1];
    int shm_fd = shm_open(name, O_RDONLY);
    ftruncate(smh_fd, MMAP_SIZE);

    buffer = (item*)(mmap(0, MMAP_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0));
    buffer[0]->item_no = 0;

    consume();

    return 0;
}
