
typedef struct pcb
{   
    unsigned int pid;
    void *outermost_page_base_address;
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