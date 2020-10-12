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


/* produce an item in next_produced */
/* 1. Increment the buffer count (item_no)  */
/* 2. Calculate the 16-bit checksum (cksum) */
/* 3. Generate the payload data             */
/*    next_produced.payload[n] = (unsigned char) rand() % 256 */
void next_produced(item* item_ptr)
{
    time_t t;
    srand((unsigned) time(&t));

    for (size_t i = 0; i < PAYLOAD_SIZE; ++i)
    {
        item_ptr->payload[i]=(unsigned char) rand() % 256;
    }
    item_ptr->cksum = ip_checksum(item_ptr->payload, PAYLOAD_SIZE);
    item_ptr->item_no = ITEM_NO++;
}



void produce()
{
    int in = 0;
    int out = 0;


    item next_produced;

    while (true)
    {

        next_produced(&next_produced);


        while (((in + 1) % BUFFER_SIZE) == out)
        {
            sleep(1); /* do nothing but sleep for 1 second */
        }
        buffer[in] = next_produced; /* store next_produced into shared buffer */
        in = (in + 1) % BUFFER_SIZE;
    }
}


int main(int argc, char const **argv)
{
    char* name = argv[1];
    int shm_fd = shm_open(name, O_RDWR);
    ftruncate(smh_fd, MMAP_SIZE);

    buffer = (item*)(mmap(0, MMAP_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0));

    produce();

    return 0;
}
