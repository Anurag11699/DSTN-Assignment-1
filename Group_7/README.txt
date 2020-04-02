Group 7

Members:
Anurag Madnawat      2017A7PS1923G
Chinmay Gupta        2017A7PS0130G
Suryateja Ratakumtla 2017A7PS0113G
Madhav Sasikumar     2016B5A70479G

Interacted With:
Aboli Vijayanad Pai  2017A7PS0147G
Amol Vijayanad Pai   2017A7PS0038G


Contribution by Each Member:

Anurag Madnawat(40%): Main Memory, Kernel, driver, debugging and integrating code, EMAT calculation, contributing with ideas for code structure, planning the code    
Chinmay Gupta(20%): L2 cache, debugging, contributing with ideas for code structure, planning the code
Suryateja Ratakumtla(20%): L1 cache, debugging, contributing with ideas for code structure, planning the code
Madhav Sasikumar(20%): TLB,debugging, contributing with ideas for code structure, planning the code


List of Files:

.h files:
macros.h: Contains all the macros used in the code.
global_variables.h: Contains all the global variables used in the code.
functions.h: Contains the declarations of all the functions used in the entire code.
tlb_data_structures.h: Contains all ADTs associated with L1 and L2 TLB.
cache_data_structures.h: Contains all ADTs associated with L1 and L2 cache.
main_memory_data_structures: Contains all ADTs associated with main memory.
kernel_data_structures.h: Contains all ADTs associated with kernel.

.c files:
driver.c: File which contains main. Takes all inputs and starts the simulation.
tlb_functions.c: Contains all the functions related to the working of the L1 and L2 tlb.
cache_functions.c: Contais all the functions related to the working of the L1 and L2 cache.
main_memory_functions.c: Contains all the functions related to the working of the main memory.
kernel_functions.c: Contains all the functions related to the working of the kernel. This binds all the memory level together and is used to call the needed functions based on the situation.

.txt files:
input.txt: Contains number of processes and the input file name corresponding to the requests of each process.
OUTPUT.txt: Upon simulation, will contain the output of the program.
time_values.txt: Contains the time values for each aspect of the effective memory access time calculation.



How to Compile:
Go to the path where the folder Group_7 is stored
cd Group_7 (to go into the folder)
either type make or make all to Compile

How to Execute:
Compile the code as show above
go to Group_7 folder
Type ./test in the terminal to execute the program


