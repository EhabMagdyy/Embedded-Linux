#include "conf.h"

int main() {
    int shID = shmget(MY_KEY, SHARED_MEMORY_SIZE, IPC_CREAT | SHARED_MEMORY_PERMISSION);
    if (shID == ERROR_FLAG) {
        perror("shmget");
        return 1;
    }

    char* shmAddr = (char*)shmat(shID, NULL, 0);
    if (shmAddr == ERROR_ADDR) {
        perror("shmat");
        return 1;
    }

    int semID = semget(MY_KEY, 2, IPC_CREAT | 0600);
    if (semID == ERROR_FLAG) {
        perror("semget");
        return 1;
    }

    struct sembuf waitP2 = {SEMAPHORE_NUMBER_PROC_2, -1, SEMAPHORE_FLAG};
    struct sembuf signalP1 = {SEMAPHORE_NUMBER_PROC_1, 1, SEMAPHORE_FLAG};
    struct sembuf waitP1 = {SEMAPHORE_NUMBER_PROC_1, -1, SEMAPHORE_FLAG};
    struct sembuf signalP2 = {SEMAPHORE_NUMBER_PROC_2, 1, SEMAPHORE_FLAG};

    while (1) {
        // Wait until Process1 sends data (wait until process 1 signals)
        semop(semID, &waitP2, 1);       // decrements by 1 -> sem[1]--

        if (strcmp(shmAddr, "exit") == 0) {
            printf("Process1 exited, terminating...\n");
            semop(semID, &signalP1, 1); // let P1 break
            break;
        }

        printf("Ping: %s\n", shmAddr);

        // Respond
        printf("Pong: ");
        fgets(shmAddr, SHARED_MEMORY_SIZE, stdin);
        shmAddr[strcspn(shmAddr, "\n")] = '\0';

        if (strcmp(shmAddr, "exit") == 0) {
            semop(semID, &signalP1, 1);
            break;
        }

        // Signal Process1 to read reply
        semop(semID, &signalP1, 1);
    }

    shmdt(shmAddr);
    return 0;
}
