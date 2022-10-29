/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

#include <stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/*Read the memory traces and convert it to binary*/
uint64_t convert_address(char memory[]);
void evaluate_cache_model(int , int , int, int, char **);

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <question> <trace file name>\n", argv[0]);
        return 1;
    }

    char* trace_file_name = argv[2];
    
    if (strncmp(argv[1], "question1", 9)==0) { 
        
        long cacheSize = 32768;
        int selectCacheModel;
        printf("Please Select : \n 1. fully-associative cache \n 2. 8-way set associative cache \n 3. 4-way set associative cache \n 4. 2-way set associative cache \n 5. Exit \n");
        scanf("%d", &selectCacheModel);
        
        int cacheLineSize;
        printf("Please select Cache Line Size [16/32/128] bytes \n");
        scanf("%d", &cacheLineSize);
        
        int numOfBlocks ;
        numOfBlocks = (int)(cacheSize/cacheLineSize);
        
        switch(selectCacheModel){
        
        case 1:
             evaluate_cache_model(numOfBlocks, 1 , cacheLineSize, cacheSize,&trace_file_name);
             break;
        case 2:
             evaluate_cache_model(numOfBlocks, 8 , cacheLineSize, cacheSize,&trace_file_name);
             break;
        case 3:
              evaluate_cache_model(numOfBlocks, 4 , cacheLineSize, cacheSize,&trace_file_name);
             break;
        case 4:
              evaluate_cache_model(numOfBlocks, 2 , cacheLineSize, cacheSize,&trace_file_name);
             break;
        case 5:
             exit(0);
        default:
             printf("Invalid Entry");
        }
        }
    
    else if (strncmp(argv[1], "question2", 9)==0) {
        printf("Bye");
    }

    return 0;
}

void evaluate_cache_model(int numOfBlocks, int nWay, int cacheLineSize, int cacheSize, char **trace_file_name)
{

	/*The data structure of associative-mapped cache*/
	struct associative_mapped_cache {
	unsigned valid_field[numOfBlocks];   /* Valid field */
	unsigned dirty_field[numOfBlocks];   /* Dirty field; since we don't distinguish writes and \\
		                           reads in this project yet, this field doesn't really matter */
	uint64_t tag_field[numOfBlocks];     /* Tag field */
	char data_field[numOfBlocks][cacheLineSize];  /* Data field; since we don't really fetch data, \\
		                                 this field doesn't really matter */
	int hits;                          /* Hit count */
	int misses;                        /* Miss count */
	};

	struct associative_mapped_cache d_cache;
	//char* trace_file_name = argv[2];

        char mem_request[20];
        uint64_t address;
        FILE *fp;

	/* Initialization */
	for (int i=0; i<numOfBlocks; i++) {
	d_cache.valid_field[i] = 0;
	d_cache.dirty_field[i] = 0;
	d_cache.tag_field[i] = 0;
	}
	d_cache.hits = 0;
	d_cache.misses = 0;
	
	int noOfSets = (int) numOfBlocks / nWay;
	
	/* Opening the memory trace file */
	fp = fopen(*trace_file_name, "r");
    
        /* Read the memory request address and access the cache */
        while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
            uint64_t block_addr = address >> (unsigned)log2(cacheLineSize);
            uint64_t setNumber = block_addr % numOfBlocks;
            uint64_t tag = block_addr >> (unsigned)log2(numOfBlocks);
            
            int setStartIndex = ((int)setNumber) * nWay;
            
            int setEndIndex = setStartIndex + nWay;
            
            printf("%d \n", setEndIndex);
            int index;
            
                        
            int isMiss = 1;
            
            for( index = setStartIndex ; index < setEndIndex ; index++)
            {
             
             if(d_cache.valid_field[index] && d_cache.tag_field[index] == tag)
             {
               d_cache.hits = d_cache.hits + 1;
               isMiss = 0;
               break;
             }
            }
            
          int isEmpty = 0;
          
          if (isMiss == 1)
            {
              d_cache.misses +=1;
              
           for( index = setStartIndex ; index < setEndIndex ; index++)
            {
             
             if(d_cache.valid_field[index] == 0)
             {
               d_cache.valid_field[index] = 1;
               d_cache.tag_field[index] = tag;
               isEmpty = 1;
               break;
             }
            }
             }
            if (isEmpty == 0)
            {
              int randomNumber = rand()%((setEndIndex+1) - setStartIndex) + setStartIndex;
              d_cache.valid_field[randomNumber] = 1;
              d_cache.tag_field[randomNumber] = tag;
            }
            

        }
        /*Print out the results*/
        printf("\n==================================\n");
        printf("Cache type:    Direct-Mapped Cache\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", d_cache.hits);
        printf("Cache Misses:  %d\n", d_cache.misses);
        printf("\n");
        
      fclose(fp);
    
}
uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;

    while (memory_addr[i] != '\n') {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0') {
            binary = (binary*16) + (memory_addr[i] - '0');
        } else {
            if(memory_addr[i] == 'a' || memory_addr[i] == 'A') {
                binary = (binary*16) + 10;
            }
            if(memory_addr[i] == 'b' || memory_addr[i] == 'B') {
                binary = (binary*16) + 11;
            }
            if(memory_addr[i] == 'c' || memory_addr[i] == 'C') {
                binary = (binary*16) + 12;
            }
            if(memory_addr[i] == 'd' || memory_addr[i] == 'D') {
                binary = (binary*16) + 13;
            }
            if(memory_addr[i] == 'e' || memory_addr[i] == 'E') {
                binary = (binary*16) + 14;
            }
            if(memory_addr[i] == 'f' || memory_addr[i] == 'F') {
                binary = (binary*16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}


