#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

L1_cache* initialize_L1_cache()
{
    //L1 cache has 2^6 indexes
    L1_cache *L1_cache_object = (L1_cache *)malloc(sizeof(L1_cache));
    L1_cache_object->L1_cache_entries = (L1_cache_entry *)malloc(sizeof(L1_cache_entry)*64);

    for(int i=0;i<64;i++)
    {
        L1_cache_object->L1_cache_entries[i].valid_set_0=0;
        L1_cache_object->L1_cache_entries[i].valid_set_1=0;
        L1_cache_object->L1_cache_entries[i].valid_set_2=0;
        L1_cache_object->L1_cache_entries[i].valid_set_3=0;
    }

    return L1_cache_object;
}

L2_cache* initialize_L2_cache()
{
    //L2 cache has 2^6 indexes
    L2_cache *L2_cache_object = (L2_cache *)malloc(sizeof(L2_cache));
    L2_cache_object->L2_cache_entries = (L2_cache_entry *)malloc(sizeof(L2_cache_entry)*64);

    for(int i=0;i<64;i++)
    {
        L2_cache_object->L2_cache_entries[i].valid_set_0=0;
        L2_cache_object->L2_cache_entries[i].valid_set_1=0;
        L2_cache_object->L2_cache_entries[i].valid_set_2=0;
        L2_cache_object->L2_cache_entries[i].valid_set_3=0;
        L2_cache_object->L2_cache_entries[i].valid_set_4=0;
        L2_cache_object->L2_cache_entries[i].valid_set_5=0;
        L2_cache_object->L2_cache_entries[i].valid_set_6=0;
        L2_cache_object->L2_cache_entries[i].valid_set_7=0;
        L2_cache_object->L2_cache_entries[i].valid_set_8=0;
        L2_cache_object->L2_cache_entries[i].valid_set_9=0;
        L2_cache_object->L2_cache_entries[i].valid_set_10=0;
        L2_cache_object->L2_cache_entries[i].valid_set_11=0;
        L2_cache_object->L2_cache_entries[i].valid_set_12=0;
        L2_cache_object->L2_cache_entries[i].valid_set_13=0;
        L2_cache_object->L2_cache_entries[i].valid_set_14=0;
        L2_cache_object->L2_cache_entries[i].valid_set_15=0;
    }

    return L2_cache_object;
}