## Task_5 Shared Memory (Inter Process Communication)

### Demo
> https://github.com/user-attachments/assets/b303d0cc-baec-429d-a5c1-66bc525a89db

---

### 📘 Overview

This project demonstrates **Inter-Process Communication (IPC)** in Linux using **System V Shared Memory** and **Semaphores**.  
Two processes (`process1` and `process2`) communicate through a shared memory buffer in a **ping-pong** style:

- **Process 1 (Ping)** sends a message first, then waits for a reply.
- **Process 2 (Pong)** waits for Process 1’s message, prints it, and sends a response back.

Synchronization between both processes is achieved using **two semaphores**, ensuring that:
- Only one process writes or reads at a time.
- Each process waits for the other’s signal before proceeding.
- Messages alternate correctly without race conditions.

---

### ⚙️ Features
- Bidirectional communication between two processes.  
- Proper synchronization using System V semaphores (`semget`, `semop`, `semctl`).  
- Shared memory (`shmget`, `shmat`, `shmdt`) used as a common data buffer.  
- Safe termination when either process sends `"exit"`.  
- Demonstrates core IPC concepts: mutual exclusion, synchronization, signaling, and memory sharing.

---

### 🧱 Architecture

#### Shared Memory
Acts as the **data exchange buffer** between processes.
- Created using `shmget()` with a unique key.
- Attached to each process using `shmat()`.
- Size: 100 bytes.

#### Semaphores
Used to synchronize access to the shared memory.
- Two semaphores are created:
  - `sem[0]` → Controls **Process 1** (Ping).
  - `sem[1]` → Controls **Process 2** (Pong).
- Semaphore operations:
  - `sem_op = -1` → **Wait** (block if value = 0).
  - `sem_op = +1` → **Signal** (increment and wake up waiting process).

---

### 🔁 Communication Sequence

| Step | Action                           | sem[0] | sem[1] |
| ---- | -------------------------------- | ------ | ------ |
| 1    | Process 1 starts, sends message  | 0      | 1      |
| 2    | Process 2 reads message, replies | 1      | 0      |
| 3    | Repeat ping-pong exchange        | ↕      | ↕      |

The exchange continues until either process sends `"exit"`, signaling both processes to terminate gracefully.

---

### 🚀 How to Run

1. **Compile both programs:**
   
   ```bash
   gcc process1.c -o p1.o
   gcc process2.c -o p2.o
   ```

2. **Run process1 first, then process 2 in two terminals**

   ```bash
   ./p1.o
   ```

   ```bash
   ./p2.o
   ```

3. **To terminate:**
 Type `exit` in either process. Both processes will clean up shared memory and semaphores.

---

### 🧰 Useful Commands

##### View existing IPC objects:

```bash
ipcs
```

##### View only semaphores

```bash
ipcs -s
```

##### Remove a semaphore manually

```bash
ipcrm -s <semid>
```

##### Remove shared memory segment manually

```bash
ipcrm -m <shmid>
```

---

### 📄 Files

| File           | Description                                               |
| -------------- | --------------------------------------------------------- |
| **conf.h**     | Contains constants, macros, and shared IPC configuration. |
| **process1.c** | “Ping” process — sends first, then waits for response.    |
| **process2.c** | “Pong” process — waits first, then replies.               |
| **README.md**  | Documentation file describing the project.                |

---

### ⚙️ IPC Functions Explained

#### 🧩 Shared Memory Functions

##### `int shmget(key_t key, size_t size, int shmflg)`
- **Purpose:** Creates a new shared memory segment or accesses an existing one.
- **Parameters:**
  - `key`: Unique identifier for the shared memory segment.
  - `size`: Size (in bytes) of the memory segment.
  - `shmflg`: Flags to control permissions and creation (e.g., `IPC_CREAT | 0666`).
- **Returns:** Shared memory ID (`shmid`) on success, `-1` on failure.

---

##### `void *shmat(int shmid, const void *shmaddr, int shmflg)`
- **Purpose:** Attaches the shared memory segment to the process’s address space.
- **Parameters:**
  - `shmid`: ID returned by `shmget`.
  - `shmaddr`: Desired address to attach (usually `NULL` for automatic choice).
  - `shmflg`: Flags for access (e.g., `0` for read/write, `SHM_RDONLY` for read-only).
- **Returns:** Pointer to the attached shared memory on success, `(void *) -1` on failure.

---

##### `int shmdt(const void *shmaddr)`
- **Purpose:** Detaches the shared memory segment from the process.
- **Parameters:**
  - `shmaddr`: Address returned by `shmat`.
- **Returns:** `0` on success, `-1` on failure.

---

##### `int shmctl(int shmid, int cmd, struct shmid_ds *buf)`
- **Purpose:** Performs control operations on the shared memory segment (e.g., remove it).
- **Common Commands:**
  - `IPC_RMID` → Removes the shared memory segment.
  - `IPC_STAT` → Gets segment info.
- **Returns:** `0` on success, `-1` on failure.

---

#### 🔒 Semaphore Functions

##### `int semget(key_t key, int nsems, int semflg)`
- **Purpose:** Creates or accesses a semaphore set.
- **Parameters:**
  - `key`: Unique key to identify the semaphore set.
  - `nsems`: Number of semaphores in the set.
  - `semflg`: Flags and permissions (e.g., `IPC_CREAT | 0666`).
- **Returns:** Semaphore ID (`semid`) on success, `-1` on failure.

---

##### `int semctl(int semid, int semnum, int cmd, ...)`
- **Purpose:** Controls or configures semaphore operations.
- **Common Commands:**
  - `SETVAL` → Sets the semaphore’s initial value.
  - `IPC_RMID` → Removes the semaphore set.
- **Returns:** Depends on command; usually `0` on success, `-1` on failure.

---

##### `int semop(int semid, struct sembuf *sops, size_t nsops)`
- **Purpose:** Performs semaphore operations like wait (`-1`) or signal (`+1`).
- **Parameters:**
  - `semid`: ID of the semaphore set.
  - `sops`: Pointer to an array of `struct sembuf` defining operations.
  - `nsops`: Number of operations to perform.
- **Returns:** `0` on success, `-1` on failure.
- **Behavior:**
  - `sem_op = -1` → **Wait (P)** — decrements the semaphore; blocks if it’s `0`.
  - `sem_op = +1` → **Signal (V)** — increments the semaphore.
  - `sem_op = 0`  → Waits until the semaphore value becomes `0`.

---

#### 🧠 Summary

| Function | Purpose                                   | Category      |
| -------- | ----------------------------------------- | ------------- |
| `shmget` | Create/get shared memory                  | Shared Memory |
| `shmat`  | Attach shared memory to process           | Shared Memory |
| `shmdt`  | Detach shared memory                      | Shared Memory |
| `shmctl` | Control shared memory (remove, info)      | Shared Memory |
| `semget` | Create/get semaphore set                  | Semaphore     |
| `semctl` | Control semaphore (init/remove)           | Semaphore     |
| `semop`  | Perform semaphore operation (wait/signal) | Semaphore     |
