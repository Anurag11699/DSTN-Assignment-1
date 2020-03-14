#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"


int main()
{

   main_memory* main_memory_32MB = initialize_main_memory(32,1); 
   tlb* L1_tlb = initialize_tlb(12);
   tlb *L2_tlb = initialize_tlb(24);
   L1_cache* L1_cache_8KB = initialize_L1_cache();
   L2_cache* L2_cache_32KB = initialize_L2_cache();
   L2_cache_write_buffer* L2_cache_write_buffer_8 = initialize_write_buffer();

   main_memory_32MB->pcb_array[0].fd=fopen("APSI.txt","r");
   main_memory_32MB->pcb_array[1].fd=fopen("CC1.txt","r");
   main_memory_32MB->pcb_array[2].fd=fopen("LI.txt","r");
   main_memory_32MB->pcb_array[3].fd=fopen("M88KSIM.txt","r");
   main_memory_32MB->pcb_array[4].fd=fopen("VORTEX.txt","r");

   
}

