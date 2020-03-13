#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

tlb* initialize_tlb(int number_of_entries)
{
    tlb *tlb_object = (tlb *)malloc(sizeof(tlb));
    tlb_object->tlb_entries=(tlb_entry *)malloc(sizeof(tlb_entry)*number_of_entries);

    return tlb_object;
}

void tlb_flush(tlb* tlb_object, int number_of_entries)
{
    for(int i=0;i<number_of_entries;i++)
    {
        tlb_object->tlb_entries[i].valid=0;
    }
}