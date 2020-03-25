#ifndef KERNEL_DATA_STRUCTURES_H
#define KERNEL_DATA_STRUCTURES_H

typedef struct pcb
{   
    unsigned int pid;
    int outer_page_base_address:15;
    int outer_page_base_address_initialized_before:1;
    FILE* fd;
    int state;
}pcb;

typedef struct kernel
{
    int currently_running_process_pid;
    int CR3_reg;
    int current_instruction;
    unsigned int number_of_processes;

    pcb* pcb_array;
}kernel;

#endif /*KERNEL_DATA_STRUCTURES_H*/