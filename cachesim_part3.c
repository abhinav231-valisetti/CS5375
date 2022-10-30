#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct l1_mapped_cache
{
    unsigned valid_field[1024];
    unsigned dirty_field[1024];
    uint64_t tag_field[1024];
    char data_field[1024][64];
    int hits;
    int misses;
};
  
struct l2_mapped_cache
{
    unsigned valid_field[16384];
    unsigned dirty_field[16384];
    uint64_t tag_field[16384];
    char data_field[16384][64];
    int hits;
    int misses;
};

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

int checkAndInsert_L1(uint64_t address, int nWay, struct l1_mapped_cache *l1,int cacheLineSize, int numOfSets)
{
    uint64_t block_addr = address >> (unsigned)log2(cacheLineSize);
    int setNumber = block_addr % numOfSets;
    uint64_t tag = block_addr >> (unsigned)log2(numOfSets);
    int setStartIndex = ((int)setNumber) * nWay;
    int setEndIndex = setStartIndex + nWay;
    
    int index;
    
    int isMiss = 0;
    for(index = setStartIndex; index < setEndIndex ; index ++)
    {
        if (l1->valid_field[index] && l1->tag_field[index] == tag)
        {
            isMiss = 1;
        }
    }
    
          int isEmpty = 0;
          
          // If the case is cache Miss try to find empty block in the cache and store the address in the particular set of cache
          if (isMiss == 1)
            {
             
           for( index = setStartIndex ; index < setEndIndex ; index++)
            {
             
             if(l1->valid_field[index] == 0)
             {
               // Store the address in the cache
               l1->valid_field[index] = 1;
               l1->tag_field[index] = tag;
               isEmpty = 1;
               break;
             }
            }
             }
            
            // If there is no empty cache block is found then untilize the random algorithm to replace the used cache block with the new address
            if (isEmpty == 0)
            {
              int randomNumber = rand()%((setEndIndex+1) - setStartIndex) + setStartIndex;
              l1->valid_field[randomNumber] = 1;
              l1->tag_field[randomNumber] = tag;
            }
    
    return isMiss;
}

int checkAndInsert_L2(uint64_t address, int nWay, struct l2_mapped_cache *l2,int cacheLineSize, int numOfSets)
{
    uint64_t block_addr = address >> (unsigned)log2(cacheLineSize);
    int setNumber = block_addr % numOfSets;
    uint64_t tag = block_addr >> (unsigned)log2(numOfSets);
    int setStartIndex = ((int)setNumber) * nWay;
    int setEndIndex = setStartIndex + nWay;
    
    int index;
    
    int isMiss = 0;
    for(index = setStartIndex; index < setEndIndex ; index ++)
    {
        if (l2->valid_field[index] && l2->tag_field[index] == tag)
        {
            isMiss = 1;
        }
    }
    
          int isEmpty = 0;
          
          // If the case is cache Miss try to find empty block in the cache and store the address in the particular set of cache
          if (isMiss == 1)
            {
             
           for( index = setStartIndex ; index < setEndIndex ; index++)
            {
             
             if(l2->valid_field[index] == 0)
             {
               // Store the address in the cache
               l2->valid_field[index] = 1;
               l2->tag_field[index] = tag;
               isEmpty = 1;
               break;
             }
            }
             }
            
            // If there is no empty cache block is found then untilize the random algorithm to replace the used cache block with the new address
            if (isEmpty == 0)
            {
              int randomNumber = rand()%((setEndIndex+1) - setStartIndex) + setStartIndex;
              l2->valid_field[randomNumber] = 1;
              l2->tag_field[randomNumber] = tag;
            }
    
    return isMiss;
}


int main(int argc, char *argv[])
{
    char *trace_file_name;
    trace_file_name = argv[2];
    
    // Initialization of blocks and sets
    int numOfBlocksinl1 = 1024;
    int numOfBlocksinl2 = 16384;
    int l1nWay = 2;
    int l2nWay = 8;
    int numOfSetsl1 = (int)(numOfBlocksinl1/l1nWay);
    int numOfSetsl2 = (int)(numOfBlocksinl2/l2nWay);
    
    // Initialization for l1 and l2 cache
    struct l1_mapped_cache l1;
    struct l2_mapped_cache l2;
    
    for (int i = 0; i < numOfBlocksinl1; i++)
    {
        l1.valid_field[i] = 0;
        l1.dirty_field[i] = 0;
        l1.tag_field[i] = 0;
    }
    
    l1.hits = 0;
    l1.misses = 0;
    
    for (int i = 0; i < numOfBlocksinl2; i++)
    {
        l2.valid_field[i] = 0;
        l2.dirty_field[i] = 0;
        l2.tag_field[i] = 0;
    }
    
    l2.hits = 0;
    l2.misses = 0;
    
    FILE *fp;
    char mem_request[20];
    uint64_t address;
    
    fp = fopen(trace_file_name, "r");
    
        while (fgets(mem_request, 20, fp) != NULL)
        {
            address = convert_address(mem_request);
            // Check For cache hit in l1
            int check_l1 = checkAndInsert_L1(address, l1nWay, &l1,64,numOfSetsl1);
            if (check_l1 == 1)
            {   
               // Increment Both hits (l1 and l2)
                l1.hits++;
                l2.hits++;
            }
            //Check For cache hit in l2 if l1 had a cache miss
            else
            {   //Increment For l1 miss
                l1.misses++;
                int check_l2 = checkAndInsert_L2(address, l2nWay, &l2,64,numOfSetsl2);
                if (check_l2)
                { 
                    //Increment For l2 Hit
                    l2.hits += 1;
                }
                else
                {   //Increment For l2 miss
                    l2.misses++;
                    
                }
            }
        }
        
        
        //output
        printf("L1 Cache\n");
        printf("============================\n");
        printf("Cache Hits: %d\n", l1.hits);
        printf("Cache Misses: %d\n", l1.misses);
        printf("\n");
        
        int totalCacheBlocks = l1.hits + l1.misses;
        
        printf("-----------------------------------\n");
     printf("Cache Hits Rate:    %.2f%%\n", ((float)l1.hits/(float)totalCacheBlocks)*100);
        printf("Cache Misses Rate:  %.2f%%\n", ((float)l1.misses/(float)totalCacheBlocks)*100); 
        
        printf("\n");
        printf("-------------------------Next------------------\n");
        printf("\n");
        
        printf("L2 Cache\n");
        printf("=============================\n");
        printf("Cache Hits: %d\n", l2.hits);
        printf("Cache Misses: %d\n", l2.misses);
        printf("\n");
        
        totalCacheBlocks = l2.hits + l2.misses;
        
        printf("-----------------------------------\n");
     printf("Cache Hits Rate:    %.2f%%\n", ((float)l2.hits/(float)totalCacheBlocks)*100);
        printf("Cache Misses Rate:  %.2f%%\n", ((float)l2.misses/(float)totalCacheBlocks)*100); 

    fclose(fp);

    return 0;
}
