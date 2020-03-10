#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

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

    //initialize free frame list;
    return main_memory_object;
}