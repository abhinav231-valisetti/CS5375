/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char *trace_file_name;

uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;

    while (memory_addr[i] != '\n')
    {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0')
        {
            binary = (binary * 16) + (memory_addr[i] - '0');
        }
        else
        {
            if (memory_addr[i] == 'a' || memory_addr[i] == 'A')
            {
                binary = (binary * 16) + 10;
            }
            if (memory_addr[i] == 'b' || memory_addr[i] == 'B')
            {
                binary = (binary * 16) + 11;
            }
            if (memory_addr[i] == 'c' || memory_addr[i] == 'C')
            {
                binary = (binary * 16) + 12;
            }
            if (memory_addr[i] == 'd' || memory_addr[i] == 'D')
            {
                binary = (binary * 16) + 13;
            }
            if (memory_addr[i] == 'e' || memory_addr[i] == 'E')
            {
                binary = (binary * 16) + 14;
            }
            if (memory_addr[i] == 'f' || memory_addr[i] == 'F')
            {
                binary = (binary * 16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}

void evaluate_cache_model(int numOfBlocks, int nWay, int cacheLineSize, int cacheSize)
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
	
	// Dividing Blocks into n sets based on nWay
	// For fully associative Mapped Cache numOfSets == numOfBlocks
	int numOfSets = (int) numOfBlocks / nWay;
	
	/* Opening the memory trace file */
	fp = fopen(trace_file_name, "r");
    
        /* Read the memory request address and access the cache */
        while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
            uint64_t block_addr = address >> (unsigned)log2(cacheLineSize);
            uint64_t setNumber = block_addr % numOfSets;
            uint64_t tag = block_addr >> (unsigned)log2(numOfSets);
            
            // Finding starting index and ending index of a set
            int setStartIndex = ((int)setNumber) * nWay;
            int setEndIndex = setStartIndex + nWay;
           
            int index;
                   
            int isMiss = 1;
            
            //Looping in the set of Blocks to find for a cache Hit
            for( index = setStartIndex ; index < setEndIndex ; index++)
            {
             
             if(d_cache.valid_field[index] && d_cache.tag_field[index] == tag)
             {
               // Increment cache Hit when address found in cache
               d_cache.hits = d_cache.hits + 1;
               isMiss = 0;
               break;
             }
            }
            
          int isEmpty = 0;
          
          // If the case is cache Miss try to find empty block in the cache and store the address in the particular set of cache
          if (isMiss == 1)
            {
              d_cache.misses +=1;
              
           for( index = setStartIndex ; index < setEndIndex ; index++)
            {
             
             if(d_cache.valid_field[index] == 0)
             {
               // Store the address in the cache
               d_cache.valid_field[index] = 1;
               d_cache.tag_field[index] = tag;
               isEmpty = 1;
               break;
             }
            }
             }
            
            // If there is no empty cache block is found then untilize the random algorithm to replace the used cache block with the new address
            if (isEmpty == 0)
            {
              int randomNumber = rand()%((setEndIndex+1) - setStartIndex) + setStartIndex;
              d_cache.valid_field[randomNumber] = 1;
              d_cache.tag_field[randomNumber] = tag;
            }
        }
        
        
        /*Print out the results*/
        printf("\n==================================\n");
        printf("Cache type: Assosiative Cache %d Way\n",nWay);
        printf("==================================\n");
        printf("Cache Hits:    %d\n", d_cache.hits);
        printf("Cache Misses:  %d\n", d_cache.misses);
        printf("\n");
        
        int totalCacheBlocks = d_cache.hits + d_cache.misses;
        
        printf("-----------------------------------\n");
     printf("Cache Hits Rate:    %.2f%%\n", ((float)d_cache.hits/(float)totalCacheBlocks)*100);
        printf("Cache Misses Rate:  %.2f%%\n", ((float)d_cache.misses/(float)totalCacheBlocks)*100);  
        
      fclose(fp);
}


int main(int argc, char *argv[])
{


    // We have two questions in part 2, used switch case to select in between those
    trace_file_name = argv[1];
    int question;
    printf("Select: \n 1. Question 1 \n 2.Question 2\n");
    scanf("%d", &question);
    
    
    switch (question)
    {
    // Question 1 with fixed cache size 32 KB
    case 1:
        printf("================= Question 1 : Fixed cache Size 32KB =================\n");
        printf("Please Select : \n 1. fully-associative cache \n 2. 8-way set associative cache \n 3. 4-way set associative cache \n 4. 2-way set associative cache \n 5. Exit \n");
        
        long cacheSize = 32768;
        int selectCacheModel;
        scanf("%d", &selectCacheModel);
        
        //Select between different Caches Line Sizes
        int cacheLineSize;
        printf("Please select Cache Line Size [16/32/128] bytes \n");
        scanf("%d", &cacheLineSize);
        
        // Formula to find total number of blocks in cache
        int numOfBlocks ;
        numOfBlocks = (int)(cacheSize/cacheLineSize);
        
         // Switch Case to select Different Cache Model
        switch(selectCacheModel){
        
        case 1:
             evaluate_cache_model(numOfBlocks, numOfBlocks , cacheLineSize, cacheSize);
             break;
        case 2:
             evaluate_cache_model(numOfBlocks, 8 , cacheLineSize, cacheSize);
             break;
        case 3:
              evaluate_cache_model(numOfBlocks, 4 , cacheLineSize, cacheSize);
             break;
        case 4:
              evaluate_cache_model(numOfBlocks, 2 , cacheLineSize, cacheSize);
             break;
        case 5:
             exit(0);
        default:
             printf("Invalid Entry");
        }
        
        break;


// Question 2 with fixed Cache Line Size of 64 Bytes

    case 2:
    printf("================= Question 2 : Fixed cache Line Size 64 Bytes =================\n");
        printf("Please Select : \n 1. fully-associative cache \n 2. 8-way set associative cache \n 3. 4-way set associative cache \n 4. 2-way set associative cache \n 5. Exit \n");
        int cacheLineSize_q2 = 64;
        int selectCacheModel_q2;
        scanf("%d", &selectCacheModel_q2);
        
        //Select between different Caches Sizes
        int cacheSize_q2;
        printf("Please select Cache Size [16/32/64] KB \n");
        scanf("%d", &cacheSize_q2);
        
        // Conversion of Cache size
        cacheSize_q2 = cacheSize_q2 * 1024; 
        
        // Formula to find total number of blocks in cache
        int numOfBlocks_q2;
        numOfBlocks_q2 = (int)(cacheSize_q2/cacheLineSize_q2);
        
        
        // Switch Case to select Different Cache Model
        switch(selectCacheModel_q2){
        
        case 1:
             evaluate_cache_model(numOfBlocks_q2, numOfBlocks_q2 , cacheLineSize_q2, cacheSize_q2);
             break;
        case 2:
             evaluate_cache_model(numOfBlocks_q2, 8 , cacheLineSize_q2, cacheSize_q2);
             break;
        case 3:
              evaluate_cache_model(numOfBlocks_q2, 4 , cacheLineSize_q2, cacheSize_q2);
             break;
        case 4:
              evaluate_cache_model(numOfBlocks_q2, 2 , cacheLineSize_q2, cacheSize_q2);
             break;
        case 5:
             exit(0);
        default:
             printf("Invalid Entry");
        }
        break;
    }

    }

