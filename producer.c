#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>


#define MMAP_SIZE 4096
#define BUFFER_SIZE 100
#define PAYLOAD_SIZE 34

typedef struct
{
     int item_no;                           /* number of the item produced */
     unsigned short cksum;                  /* 16-bit Internet checksum*/
     unsigned char payload[PAYLOAD_SIZE];   /* random generated data */
} item;

item buffer[BUFFER_SIZE];
int ITEM_NO = 0;
const char* name;

void handle_sigint(int sig)
{
    shm_unlink(name);
    exit(0);
}

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
void func_produce(item* item_ptr)
{

    for (size_t i = 0; i < PAYLOAD_SIZE; ++i)
    {
        item_ptr->payload[i]=(unsigned char) rand() % 256;
    }
    printf("%s\n", item_ptr->payload);
    item_ptr->cksum = ip_checksum(item_ptr->payload, PAYLOAD_SIZE);
    item_ptr->item_no = ITEM_NO++;
}



void produce(item* ptr)
{
    int in = 0;
    int out = 0;
    bool f1 = true; //finished 1 time

    item next_produced;

    while (true)
    {

        func_produce(&next_produced);


        while (((in + 1) % BUFFER_SIZE) == out)
        {
            if (f1)
            {
                printf("done!\n");
                f1 = false;
            }
            sleep(1); /* do nothing but sleep for 1 second */
        }
        buffer[in] = next_produced; /* store next_produced into shared buffer */
        memcpy(&(ptr[in]), &buffer[in], PAYLOAD_SIZE);
        in = (in + 1) % BUFFER_SIZE;
    }
}


int main(int argc, char const **argv)
{
    name = argv[1];

    signal(SIGINT, handle_sigint);


    printf("name:%s\n", name);
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd==-1)
    {
        printf("error opening shared memory\nname:%s errno:%d\n", name, errno);
        exit(1);
    }
    ftruncate(shm_fd, MMAP_SIZE);

    item* ptr = (item*)(mmap(0, MMAP_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0));


    produce(ptr);
    return 0;
}
