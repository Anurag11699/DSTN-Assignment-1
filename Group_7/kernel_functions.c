#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

kernel* initialize_kernel()
{
    kernel* kernel_object = (kernel *)malloc(sizeof(kernel));
    
    return kernel_object;
}