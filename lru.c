#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * functions initialization
 */

int lruAlgo(const int *time, int n);
int virtualMem(int no_pages, int frame_size);
int wordSize(int bits); // # 8, 16, 32, 64
void memorySize(int size);
int cacheSize(int size);
void pageSize(int size);
void sharedLibrary(int sid, int memoryspace);
void process(int pid, int memoryspace, int sid);
void reference(int pid, char *startAddress, int size);
void terminate(int pid);


int execute(char ch);

// main execution
int main(int argc, char ** argv)
{
    /*
     * obtain number of pages and frames from
     * command line arguments
     */

    FILE* ptr;
    char ch;

    if(argc < 2){
        printf("\nFile is required! \n");
        exit(0);
    }
    char * filename = "";
    filename = argv[1];
    // Opening file in reading mode
    ptr = fopen(filename, "r");

    do {
        ch = fgetc(ptr);
        printf("%c", ch);
        execute(ch); // call function to execute commands
        // ! EOF
    } while (ch != EOF);

    // Closing the file
    fclose(ptr);

    int number_of_pages = 2;
    int number_of_frames = 3;
    int size = 100;
    int pid = fork();
    if(pid == 0){
        // call shared library method
        int memoryspace = 128;
        sharedLibrary(0, memoryspace);
    }
    return 0;
}

int execute(char ch) {
    // system call
    char command = ch;
    return 0;
}

int virtualMem(int no_pages, int frame_size) {
    /*
     * implementation of virtual memory
     * and caching mechanism using
     * lru algorithm
     */
    int counter = 0; // initialize counter
    int in,f, out, page_faults = 0;
    int frames[10], frame_pages[30], time[10], shared[20][20];
    char x[20];
    float fault_ratio, hit_ratio; // initialize double values for hit and fault ratios
    int  i, j, pos;

    frame_size = 2;

    // prompt for frame size values
    // space separated values
    for(i = 0; i < no_pages; i++)
    {

        scanf("%d", &frame_pages[i]);
    }
    printf("\n");

    for(i=0; i< no_pages; i++)
    {
        x[i]='X'; // hit
    }
    for(i = 0; i < frame_size; ++i)
    {
        frames[i] = 0; // initialize each frame
    }

    for(i = 0; i < no_pages; ++i)
    {
        in = out = 0;

        for(j = 0; j < frame_size; ++j)
        {
            // compare index of current frame and page
            if(frames[j] == frame_pages[i])
            {
                counter++; // increment counter
                time[j] = counter;
                in = out = 1;
                page_faults--; // reduce number of page faults fore each frame
                x[i]='*';
                page_faults++; // increment number of page faults
                break;

            }
        }

        if(in == 0)
        {
            for(j = 0; j < frame_size; ++j)
            {
                if(frames[j] == 0)
                {
                    // increment counter for each frame with a value of 0
                    // increment page fault for each frame with a value of 0
                    counter++;
                    page_faults++;
                    frames[j] = frame_pages[i];
                    time[j] = counter;
                    out = 1;
                    // exit smooth
                    break;
                }
            }
        }

        if(out == 0)
        {
            pos = lruAlgo(time, frame_size); // LRU Algorithm call
            counter++;
            page_faults++;
            // frame position and frame page
            frames[pos] = frame_pages[i];
            time[pos] = counter;
        }

        for(j = 0; j < frame_size; j++)
        {
            shared[j][i] = frames[j];
        }
    }

    printf("\n");
    for(i = 0 ; i < no_pages * 5 + 2 * frame_size + 1; i ++)
    {
        printf("-");
    }

    printf("\n|       |");
    for(i=1;i<=(no_pages*4)/2;i++)
    {
        printf(" ");
    }
    printf("Pages");
    // number of pages
    for(i=(no_pages*4)/2;i<=(no_pages*4)+13;i++)
    {
        printf(" ");
    }

    printf("|\n");
    printf("|Frames |");

    for(i = 0 ; i < no_pages*5+2*frame_size - 8; i ++)
    {
        printf("-");
    }
    printf("\n|\t|");

    for(i=0 ; i < no_pages ; i++)
    {
        // print frame pages
        printf(" %2d |",frame_pages[i]);
    }
    printf("\n");
    for(i = 0 ; i < no_pages*5+2*frame_size+1 ; i ++)
    {
        printf("-");
    }
    printf("\n");

    for(i=0;i<frame_size;i++)
    {

        printf("| %3d",i);
        printf("\t|");

        for(f=0;f<no_pages;f++)
        {

            if(shared[i][f]==0)
            {
                printf("  - |");
            }
            else
            {
                printf(" %2d |",shared[i][f]);
            }
        }

        printf("\n");
    }

    for(i = 0 ; i < no_pages * 5 + 2 * frame_size + 1; i ++)
    {
        printf("-");
    }

    printf("\n|       |");
    for(i = 0; i< no_pages; i++)
    {
        if(x[i]=='X')
        {
            printf("\033[0;31m");
            printf(" %2c ",x[i]);
            printf("\033[0m");
            printf("|");
        }
        else
        {
            printf("\033[0;32m");
            printf(" %2c ",x[i]);
            printf("\033[0m");
            printf("|");
        }
    }

    printf("\n");
    for(i = 0 ; i < no_pages * 5 + 2 * frame_size + 1; i ++)
    {
        printf("-");
    }

    /*
     * display output to the console
     * 1. page faults
     * 2. fault ratio
     * 3. hit ratio
     */
    printf("\n\nPage Faults = %d\n",page_faults);

    printf("\nPage Hits = %d\n",(no_pages-page_faults));

    fault_ratio = ((float)page_faults/no_pages);
    printf("\nPage Fault ratio = %5.2f   \n", fault_ratio);

    hit_ratio = ((float)no_pages-page_faults)/no_pages;

    printf("\nPage Hit ratio = %5.2f   \n", hit_ratio);

    // exit cleanly
    return 0;
}


int lruAlgo(const int *time, int n)
{
    /*
     * implementation of LRU algorithm
     */
    int i, minimum = time[0], position = 0;

    // iterate through the size of the frame
    for(i = 1; i < n; ++i)
    {
        if(time[i] < minimum)
        {
            minimum = time[i];
            position = i; //position
        }
    }
    // return pos as the main arg
    return position;
}


int wordSize(int bits) {
    int time = 0;
    return lruAlgo(&time, bits);
}

void memorySize(int size){
    if(wordSize(size) > 10){
        // init memory
        int mem = 1000;
        int cache = 10;
        for(int i = 0; i < mem; i += 1){
            cacheSize(cache);
        }
    }
}

int cacheSize(int size) {
    return size;
}

void sharedLibrary(int sid, int memoryspace) {
    // method implementation
    for(int x = 0; x < 2; x++){ printf("Shared library(%d, %d)\n", sid, memoryspace); }
    // call process method
    int pid = fork();
    process(pid, memoryspace, sid);
}
void process(int pid, int memoryspace, int sid){
    if(pid == 0){
        // child process
//        printf("Process(%d, %d, %d)\n", sid, memory space, pid);
    }printf("Process(%d, %d, %d)\n", sid, memoryspace, pid);
    int size = 8;
    for(int i = 0; i < size; i++){
        printf("# cache miss, page miss, load page 0 of pid=%d to mem[%d], then to cache[%d]\n", pid, i, (i + 1));
        reference(pid, "0x10c", 5128);
    }

}
void reference(int pid, char *startAddress, int size){
    printf("reference(%d , %s , %d)\n", pid, startAddress, size);
    terminate(pid);
}

void terminate(int pid){
    // exit
    printf("terminate(%d)", pid);
}

void pageSize(int size){
    if(wordSize(size) > 1){
        // init memory
        memorySize(size);
    }
    sharedLibrary(0, 1024);
    terminate(0)
}
