#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

void add_free_frame(main_memory* main_memory_object, int frame_number)
{
    //free_frame *temp_tail = *tail;

    free_frame *new_frame = (free_frame *)malloc(sizeof(free_frame));
    new_frame->frame_number=frame_number;
    new_frame->next=NULL;
    main_memory_object->ffl_dummy_head->number_free_frames++;
    //head->number_free_frames++;
    if(main_memory_object->ffl_dummy_head->next==NULL)
    {
        main_memory_object->ffl_dummy_head->next=new_frame;
        main_memory_object->ffl_tail=new_frame;
        //*tail = temp_tail;
        return;
    }
    main_memory_object->ffl_tail->next=new_frame;
    main_memory_object->ffl_tail=new_frame;
    //temp_tail->next=new_frame;
    //temp_tail=new_frame;
    //*tail = temp_tail;
}

int remove_free_frame(main_memory* main_memory_object)
{

    if( main_memory_object->ffl_dummy_head->next==NULL)
    {
        return -1; //no free frame to return;
    }

    int frame_number =  main_memory_object->ffl_dummy_head->next->frame_number;
    
    free_frame * temp =  main_memory_object->ffl_dummy_head->next;
    main_memory_object->ffl_dummy_head->next= main_memory_object->ffl_dummy_head->next->next;
    free(temp);
    main_memory_object->ffl_dummy_head->number_free_frames--;

    return frame_number;
}

void add_used_frame(main_memory* main_memory_object, int frame_number)
{
    //used_frame *temp_recently_used_frame = *recently_used_frame;

    used_frame *new_frame = (used_frame *)malloc(sizeof(free_frame));
    new_frame->frame_number=frame_number;
    new_frame->reference_bit=0;
    new_frame->next=NULL;
    main_memory_object->ufl_dummy_head->number_used_frames++;
    //head->number_used_frames++;

    if(main_memory_object->ufl_dummy_head->next==NULL)
    {
        main_memory_object->ufl_dummy_head->next=new_frame;
        new_frame->next=new_frame;

        main_memory_object->recently_used_frame=new_frame;
        //*recently_used_frame=temp_recently_used_frame;
        return;
    }

    new_frame->next=main_memory_object->recently_used_frame->next;
    main_memory_object->recently_used_frame->next=new_frame;
    main_memory_object->recently_used_frame=new_frame;

    //*recently_used_frame=temp_recently_used_frame;

}

int remove_used_frame(main_memory* main_memory_object)
{
    if(main_memory_object->ufl_dummy_head->next==NULL)
    {
        return -1;
    }

    //used_frame *temp_recently_used_frame = *recently_used_frame;

    while(main_memory_object->recently_used_frame->next->reference_bit!=0)
    {
        main_memory_object->recently_used_frame->next->reference_bit=0;
        main_memory_object->recently_used_frame=main_memory_object->recently_used_frame->next;
    }

    int frame_number = main_memory_object->recently_used_frame->next->frame_number;

    used_frame *temp = main_memory_object->recently_used_frame->next;
    main_memory_object->recently_used_frame->next=main_memory_object->recently_used_frame->next->next;
    free(temp);

    main_memory_object->ufl_dummy_head->number_used_frames--;
    //*recently_used_frame=temp_recently_used_frame;
    return frame_number;

    //need to update the page tables for the process from which this frame was removed. Will be done later in another function elsewhere. 
}

int get_frame(main_memory *main_memory_object)
{
    //prefer placement over replacement
    int frame_number = remove_free_frame(main_memory_object);

    if(frame_number!=-1)
    {
        return frame_number;
    }

    frame_number = remove_used_frame(main_memory_object);

    //need to invalidate this entry in 
    return frame_number;
}

void initialize_page_table(int frame_number_occupied)
{
    int i;
    page_table* page_table_object = (page_table*)malloc(sizeof(page_table));
    page_table_object->frame_occupied=frame_number_occupied;
    page_table_object->page_table_entries = (page_table_entry*)malloc(sizeof(page_table_entry)*256); //there are 2^8 entries in each page table

    for(i=0;i<256;i++)
    {
        page_table_object->page_table_entries[i].cache_disabled=1;
        page_table_object->page_table_entries[i].frame_base_address=-1;
        page_table_object->page_table_entries[i].referenced=0;
        page_table_object->page_table_entries[i].modified=0;
    }

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

    //initialize the frames
    //main_memory_object->frame_array=(frame *)malloc(number_of_frames*sizeof(frame));

    //initialize frame table
    main_memory_object->frame_table=(frame_table_entry *)malloc(number_of_frames*sizeof(frame_table_entry));

    //all pages are invalid upon initialization
    for(int frame_number=0;frame_number<number_of_frames;frame_number++)
    {
        main_memory_object->frame_table[number_of_frames].pid=-1;
    }

    //initialize free frame list
    main_memory_object->ffl_dummy_head = (free_frame_list_dummy_head *)malloc(sizeof(free_frame_list_dummy_head));
    main_memory_object->ffl_dummy_head->next=NULL;
    main_memory_object->ffl_dummy_head->number_free_frames=0;
    main_memory_object->ffl_tail=NULL;

    //initialize used frame list
    main_memory_object->ufl_dummy_head=(used_frame_list_dummy_head *)malloc(sizeof(used_frame_list_dummy_head));
    main_memory_object->ufl_dummy_head->next=NULL;
    main_memory_object->ufl_dummy_head->number_used_frames=0;
    main_memory_object->recently_used_frame=NULL;
    

    main_memory_object->ffl_dummy_head->number_free_frames=number_of_frames; //all frames are initially free;

    //add all frames to the free frame list
    for(int frame_number=0;frame_number<number_of_frames;frame_number++)
    {
        if(main_memory_object->frame_table[frame_number].pid==-1)
        {
            
            add_free_frame(main_memory_object,frame_number);
            fprintf(stderr,"Tail: %d\n",main_memory_object->ffl_tail->frame_number);
        }
        
    }
    return main_memory_object;
}
