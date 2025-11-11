#include "conf.h"

int main(){
    // Create shared memory id
    int shID = shmget(MY_KEY, SHARED_MEMORY_SIZE, IPC_CREAT | SHARED_MEMORY_PERMISSION);
    if(shID == ERROR_FLAG){
        perror("shmget");
        return 1;
    }
    // Get shared memory buffer
    char* shmAddr =(char*)shmat(shID, NULL, 0);
    if(shmAddr == ERROR_ADDR){
        perror("shmat");
        return 1;
    }
    // Create semaphore id of size 2
    int semID = semget(MY_KEY, 2, IPC_CREAT | 0600);
    if(semID == ERROR_FLAG){
        perror("semget");
        return 1;
    }

    // Initialize semaphores -> Process 1 start first
    semctl(semID, SEMAPHORE_NUMBER_PROC_1, SETVAL, 1);  // P1 send first -> sem[0] = 1
    semctl(semID, SEMAPHORE_NUMBER_PROC_2, SETVAL, 0);  // P2 waits      -> sem[1] = 0

    struct sembuf waitP1 = {SEMAPHORE_NUMBER_PROC_1, -1, SEMAPHORE_FLAG};  // sem[0] = wait
    struct sembuf signalP2 = {SEMAPHORE_NUMBER_PROC_2, 1, SEMAPHORE_FLAG}; // sem[1] = signal
    struct sembuf waitP2 = {SEMAPHORE_NUMBER_PROC_2, -1, SEMAPHORE_FLAG};  // sem[1] = wait
    struct sembuf signalP1 = {SEMAPHORE_NUMBER_PROC_1, 1, SEMAPHORE_FLAG}; // sem[0] = signal

    while(1){
        // Wait until it's Process1's turn(until process 2 signals)
        semop(semID, &waitP1, 1);   // decremnt by 1 -> sem[0]-- "sem[0] = 0"

        // Read message to be sent to process 2 & Write it in the buffer
        printf("Ping: ");
        fgets(shmAddr, SHARED_MEMORY_SIZE, stdin);
        shmAddr[strcspn(shmAddr, "\n")] = '\0';
        // if "exit" -> terminate
        if(strcmp(shmAddr, "exit") == 0){
            semop(semID, &signalP2, 1); // Tell P2 to "exit"
            break;
        }

        // Signal Process2 that message is ready to read it
        semop(semID, &signalP2, 1);

        // Wait for Process2’s reply
        semop(semID, &waitP1, 1);       // "sem[0] = -1"

        if(strcmp(shmAddr, "exit") == 0){
            printf("Process2 exited, terminating...\n");
            break;
        }

        printf("Pong: %s\n", shmAddr);

        semop(semID, &signalP1, 1);
    }

    // Detach the shared memory segment
    shmdt(shmAddr);
    // When you detach from shared memory, it is not destroyed. So, to destroy shmctl() is used
    shmctl(shID, IPC_RMID, NULL);
    semctl(semID, 0, IPC_RMID);
    return 0;
}
