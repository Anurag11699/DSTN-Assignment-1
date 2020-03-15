CC=gcc

flags=-c -Wall

executableName=test

driver=driver

#all: DSTN

all: $(driver).o kernel_functions.o tlb_functions.o cache_functions.o main_memory_functions.o
	$(CC)  $(driver).o kernel_functions.o tlb_functions.o cache_functions.o main_memory_functions.o -o $(executableName)

$(driver).o: $(driver).c
	$(CC) $(flags) $(driver).c

kernel_functions.o: kernel_functions.c
	$(CC) $(flags) kernel_functions.c

tlb_functions.o: tlb_functions.c
	$(CC) $(flags) tlb_functions.c

cache_functions.o: cache_functions.c
	$(CC) $(flags) cache_functions.c

main_memory_functions.o: main_memory_functions.c
	$(CC) $(flags) main_memory_functions.c

clean:
	rm -f *.o $(executableName)
