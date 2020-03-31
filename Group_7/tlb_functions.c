#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

/*
PreConditions
Inputs:{number of entries in the tlb}

Purpose of the Function: Initialize the tlb data structure which has number of entries as specified by the input parameter

PostConditions
Output: {pointer to intialized tlb}
*/
tlb* initialize_tlb(int number_of_entries)
{
    tlb *tlb_object = (tlb *)malloc(sizeof(tlb));
    tlb_object->number_of_entries=number_of_entries;
    tlb_object->tlb_entries=(tlb_entry *)malloc(sizeof(tlb_entry)*number_of_entries);

    tlb_flush(tlb_object);

    return tlb_object;
}


/*
PreConditions
Inputs:{pointer to tlb object}

Purpose of the Function: Invalidate all the entries in the tlb

PostConditions
Updated tlb object
*/
void tlb_flush(tlb* tlb_object)
{
    int number_of_entries = tlb_object->number_of_entries;
    for(int i=0;i<number_of_entries;i++)
    {
        tlb_object->tlb_entries[i].valid=0;
        tlb_object->tlb_entries[i].LRU_counter=0;
    }
}


/*
PreConditions
Inputs:{pointer to tlb object, logical page number to search for}
0<=logical page number<2^22

Purpose of the Function: The function goes through the tlb to search for the entry corresponding to the given logical page number. If found, it returns the Physical frame number associated with it and sets the MSB of the counter of that entry to 1.
The function also right shifts the counters of all entries.

PostConditions
Return Value:
Physical Frame Number, if entry exists
-1, if entry does not exist
*/
int L1_tlb_search(tlb* tlb_object, int logical_page_number)
{
    number_of_L1_tlb_searches++;
    int number_of_entries = tlb_object->number_of_entries;
    //shift all the counters to the right by 1
    int i;
    
    //right shift tlbs after a certain number of acesses only
    if((number_of_L1_tlb_searches%right_shift_L1_tlb_counter) == 0)
    {
        for(i=0;i<number_of_entries;i++)
        {
            tlb_object->tlb_entries[i].LRU_counter=(tlb_object->tlb_entries[i].LRU_counter>>1);
        }
    }

    for(i=0;i<number_of_entries;i++)
    {
        if(tlb_object->tlb_entries[i].valid==1 && tlb_object->tlb_entries[i].logical_page_number==logical_page_number)
        {
            //set MSB to 1 on TLB hit
            tlb_object->tlb_entries[i].LRU_counter=set_MSB_bit_8(tlb_object->tlb_entries[i].LRU_counter);
            return tlb_object->tlb_entries[i].physical_frame_number;
        }
    }

    return -1;
}


/*
PreConditions
Inputs:{pointer to tlb object, logical page number to search for}
0<=logical page number<2^22

Purpose of the Function: The function goes through the tlb to search for the entry corresponding to the given logical page number. If found, it returns the Physical frame number associated with it and sets the MSB of the counter of that entry to 1.
The function also right shifts the counters of all entries.

PostConditions
Return Value:
Physical Frame Number, if entry exists
-1, if entry does not exist
*/
int L2_tlb_search(tlb* tlb_object, int logical_page_number)
{
    number_of_L2_tlb_searches++;
    int number_of_entries = tlb_object->number_of_entries;
    //shift all the counters to the right by 1
    int i;
    
    //right shift tlbs after a certain number of acesses only
    if((number_of_L2_tlb_searches%right_shift_L2_tlb_counter) == 0)
    {
        for(i=0;i<number_of_entries;i++)
        {
            tlb_object->tlb_entries[i].LRU_counter=(tlb_object->tlb_entries[i].LRU_counter>>1);
        }
    }

    for(i=0;i<number_of_entries;i++)
    {
        if(tlb_object->tlb_entries[i].valid==1 && tlb_object->tlb_entries[i].logical_page_number==logical_page_number)
        {
            //set MSB to 1 on TLB hit
            tlb_object->tlb_entries[i].LRU_counter=set_MSB_bit_8(tlb_object->tlb_entries[i].LRU_counter);
            return tlb_object->tlb_entries[i].physical_frame_number;
        }
    }

    return -1;
}


/*
PreConditions
Inputs:{pointer to L1 tlb object, pointer to L2 object, logical page number entry corresponding to which must be transfered from L2 tlb to L1 tlb}
0<=logical page number<2^22

Purpose of the Function: The function goes through the L2 tlb to search for the entry corresponding to the given logical page number. If found, it transferes a copy of this L2 tlb entry to the L1 tlb. In the L1 tlb we first try to replace an invalid entry, if no entry is invalid, we replace the least recently used entry 

PostConditions
Updated TLB objects if transfer took place
*/
void L2_to_L1_tlb_transfer(tlb *L1_tlb, tlb *L2_tlb, int logical_page_number)
{   
    //add time take for this transfer
    total_time_taken=total_time_taken+L1_tlb_to_from_L2_tlb_transfer_time;

    int number_of_entries_L1 = L1_tlb->number_of_entries;
    int number_of_entires_L2 = L2_tlb->number_of_entries;
    tlb_entry entry_to_be_transfered;

    //find the entry that resulted in TLB hit in L2 TLB
    int i;
    for(i=0;i<number_of_entires_L2;i++)
    {
        if(L2_tlb->tlb_entries[i].valid==1 && L2_tlb->tlb_entries[i].logical_page_number==logical_page_number)
        {
            entry_to_be_transfered=L2_tlb->tlb_entries[i];
        }
    }

    //Check if there are any invalid entries in L1 tlb and replace it with that

    for(i=0;i<number_of_entries_L1;i++)
    {
        if(L1_tlb->tlb_entries[i].valid==0)
        {
            L1_tlb->tlb_entries[i]=entry_to_be_transfered;
            return;
        }
    }


    //if not, remove LRU tlb entry
    int lru_tlb_entry=-1;
    int LRU_counter_min=__INT16_MAX__;

    for(i=0;i<number_of_entries_L1;i++)
    {
        if(L1_tlb->tlb_entries[i].LRU_counter<LRU_counter_min)
        {
            LRU_counter_min=L1_tlb->tlb_entries[i].LRU_counter;
            lru_tlb_entry=i;
        }
    }

    L1_tlb->tlb_entries[lru_tlb_entry]=entry_to_be_transfered;
}


/*
PreConditions
Inputs:{pointer to L1 tlb object, pointer to L2 object, logical page number to search for, pointer to store the physical frame number}
0<=logical page number<2^22

Purpose of the Function: This function searches in L1 tlb, if the entry corresponding to the logical page number exists in L1 tlb, returns the physical frame number in the entry. If not found in L1 tlb, L2 tlb is searched. If found in L2 tlb, a copy of that entry is transfered to L1 tlb (using L2_to_L1_tlb_transfer function) and physical frame number in the entry returned. If the entry was not found in any tlb, -1 is returned.

PostConditions
Return Value:
time taken to search for the entry
*/
int complete_tlb_search(tlb *L1_tlb, tlb *L2_tlb, int logical_page_number, int *physical_frame_number)
{
    long int time_taken=0;
    //search in L1 tlb
    (*physical_frame_number) = L1_tlb_search(L1_tlb,logical_page_number);
    //add L1_tlb search time
    time_taken = time_taken + L1_tlb_search_time;

    if((*physical_frame_number)!=-1)
    {
        return time_taken;
    }

    //add L1_tlb miss overhead
    time_taken = time_taken + L1_tlb_miss_OS_overhead;

    //search in L2 tlb
    (*physical_frame_number) = L2_tlb_search(L2_tlb,logical_page_number);
    //add L2 tlb search time
    time_taken = time_taken + L2_tlb_search_time;

    if((*physical_frame_number)!=-1)
    {   
        //entry was found in L2 tlb. As tlb's are inclusive, transfer the entry to L1 tlb. Time for this added in the function itself
        L2_to_L1_tlb_transfer(L1_tlb,L2_tlb,logical_page_number);

        //physical_frame_number=tlb_search(L1_tlb,logical_page_number);

        return time_taken;
        
    }

    //L2 tlb miss
    time_taken = time_taken + L2_tlb_miss_OS_overhead;
    
    return time_taken;
}


/*
PreConditions
Inputs:{pointer to tlb object,logical page number, physical frame number}
0<=logical page number<2^22

Purpose of the Function: This function creates a new tlb entry with the input parameters and inserts it into the tlb object given. It first searches for an invalid tlb entry to replace. If not invalid tlb entry found, it replaces the lru entry.

PostConditions
tlb object with new entry inserted
*/
void insert_new_tlb_entry(tlb* tlb_object, int logical_page_number, int physical_frame_number)
{
    int number_of_entries = tlb_object->number_of_entries;
    //Check if there are any invalid entries in tlb and replace it with that
    tlb_entry tlb_entry_to_add;
    tlb_entry_to_add.logical_page_number=logical_page_number;
    tlb_entry_to_add.physical_frame_number=physical_frame_number;
    tlb_entry_to_add.LRU_counter=0;
    tlb_entry_to_add.valid=1;

    int i;
    for(i=0;i<number_of_entries;i++)
    {
        if(tlb_object->tlb_entries[i].valid==0)
        {
            tlb_object->tlb_entries[i]=tlb_entry_to_add;
            return;
        }
    } 

    //if not, replace LRU tlb entry
    int lru_tlb_entry=-1;
    int LRU_counter_min=__INT16_MAX__;

    for(i=0;i<number_of_entries;i++)
    {
        if(tlb_object->tlb_entries[i].LRU_counter<LRU_counter_min)
        {
            LRU_counter_min=tlb_object->tlb_entries[i].LRU_counter;
            lru_tlb_entry=i;
        }
    }

    tlb_object->tlb_entries[lru_tlb_entry]=tlb_entry_to_add;
}

/*auxillary function to print a tlb object, useful for debugging*/
void print_tlb(tlb* tlb_object)
{
    int number_of_entries = tlb_object->number_of_entries;

    for(int i=0;i<number_of_entries;i++)
    {
        fprintf(output_fd,"Logical Page Number: %d | Physical Frame Number: %d | Valid: %d | LRU Counter: %d\n", tlb_object->tlb_entries[i].logical_page_number,tlb_object->tlb_entries[i].physical_frame_number, tlb_object->tlb_entries[i].valid, tlb_object->tlb_entries[i].LRU_counter);
        fflush(output_fd);
    }

    fprintf(output_fd,"\n\n");
    fflush(output_fd);
}