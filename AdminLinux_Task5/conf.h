#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MY_KEY                      13361
#define SHARED_MEMORY_SIZE          100UL
#define SHARED_MEMORY_PERMISSION    0600

#define SEMAPHORE_NUMBER_PROC_1     0
#define SEMAPHORE_NUMBER_PROC_2     1
#define SEMAPHORE_FLAG              0

#define ERROR_FLAG                  -1
#define ERROR_ADDR                  (void*)-1