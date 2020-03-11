#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"


int main()
{

   main_memory* main_memory_32MB = initialize_main_memory(32,1); 
   tlb* L1_tlb = initialize_tlb(12);
   tlb *L2_tlb = initialize_tlb(24);
}

