#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

void add_free_frame(free_frame_list_dummy_head *head, free_frame *tail, int frame_number)
{
    free_frame *new_frame = (free_frame *)malloc(sizeof(free_frame));
    new_frame->frame_number=frame_number;
    new_frame->next=NULL;
    head->number_free_frames++;

    if(head==NULL)
    {
        head->next=new_frame;
        tail=new_frame;
        return;
    }

    tail->next=new_frame;
    tail=new_frame;
}

int remove_free_frame(free_frame_list_dummy_head *head, free_frame *tail)
{
    if(head->next==NULL)
    {
        return -1; //no free frame to return;
    }

    int frame_number = head->next->frame_number;
    
    free_frame * temp = head->next;
    head->next=head->next->next;
    free(temp);

    return frame_number;
}

void add_used_frame(used_frame_list_dummy_head *head, used_frame *recently_used_frame, int frame_number)
{
    used_frame *new_frame = (used_frame *)malloc(sizeof(free_frame));
    new_frame->frame_number=frame_number;
    new_frame->reference_bit=0;
    new_frame->next=NULL;
    head->number_used_frames++;

    if(head->next==NULL)
    {
        head->next=new_frame;
        new_frame->next=new_frame;

        recently_used_frame=new_frame;
        return;
    }

    new_frame->next=recently_used_frame->next;
    recently_used_frame->next=new_frame;
    recently_used_frame=new_frame;

}

/*
PreConditions
Inputs: {main_memory_size in MB, frame_size in KB}

PostConditions
Output: {pointer to intialized main memory}
*/
main_memory* initialize_main_memory(int main_memory_size, int frame_size)
{
    //find number of frames
    int number_of_frames=(main_memory_size/frame_size)*1024;

    main_memory* main_memory_object = (main_memory *)malloc(sizeof(main_memory));

    //initialize frame table
    main_memory_object->frame_table=(frame_table_entry *)malloc(number_of_frames*sizeof(frame_table_entry));

    //all pages are invalid upon initialization
    for(int frame_number=0;frame_number<number_of_frames;frame_number++)
    {
        main_memory_object->frame_table[number_of_frames].valid_bit=0;
    }

    //initialize free frame list;
    main_memory_object->ffl_dummy_head = (free_frame_list_dummy_head *)malloc(sizeof(free_frame_list_dummy_head));
    main_memory_object->ffl_tail=NULL;
    

    main_memory_object->ffl_dummy_head->number_free_frames=number_of_frames; //all frames are initially free;

    //add all frames to the free frame list
    for(int frame_number=0;frame_number<number_of_frames;frame_number++)
    {
        if(main_memory_object->frame_table[frame_number].valid_bit==0)
        {
            add_free_frame(main_memory_object->ffl_dummy_head, main_memory_object->ffl_tail,frame_number);
        }
        
    }
    return main_memory_object;
}