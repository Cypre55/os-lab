#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

int no_of_producers;
int no_of_consumers;

// TODO: Log Steps like Produced, started, completed

using namespace std;

struct Node
{
    int job_id;
    int t2c; // In milliseconds

    int parentIdx;
    int childJobs[MAX_CHILD_JOBS];
    int numChildActive;

    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;

    // Status
    // 0: Not Started
    // 1: On Going
    // 2: Done
    int status;

};

struct SharedMem {
    int size;
    Node* nodes;
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    int _count;
    int rootIdx;

    void init() {
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&mutex, &attr);
        _count = 0;
        for (int i = 0; i < size; i++) {
            nodes[i].init();
            nodes[i].status = DONE;
        }
        rootIdx = -1;
    }
    // solution 1 to chopstick issue here: try_lock if held continue else try to acquire lock
    int addNode(Node& node) {
        PTHREAD_MUTEX_LOCK(&mutex);
        if (_count < size) {
            for (int i = 0; i < size; i++) {
                if (i == node.parentIdx)
                    continue;
                if (nodes[i].status == DONE) {
                    PTHREAD_MUTEX_LOCK(&nodes[i].mutex);
                    nodes[i] = node;
                    PTHREAD_MUTEX_UNLOCK(&nodes[i].mutex);
                    _count++;
                    if (rootIdx == -1)
                        rootIdx = i;
                    PTHREAD_MUTEX_UNLOCK(&mutex);
                    return i;
                }
            }
        }
        PTHREAD_MUTEX_UNLOCK(&mutex);
        return -1;
    }
};

// 
// int init_size = rand() % (MAX_TREE_SIZE - MIN_TREE_SIZE + 1) + MIN_TREE_SIZE;
//     int final_size = init_size + producers * (1000 / MIN_SLEEP_TIME) * (MAX_LIFETIME);

//     cout << "Creating shared memory for " << final_size << " nodes\n";
//     shmid = shmget(IPC_PRIVATE, sizeof(SharedMem), IPC_CREAT | 0666);
//     shm = (SharedMem*)shmat(shmid, NULL, 0);

//     key_t key = ftok(".", 'a');
//     shmid2 = shmget(key, sizeof(Node) * final_size, IPC_CREAT | 0666);
//     shm->nodes = (Node*)shmat(shmid2, NULL, 0);
//     shm->size = final_size;
//

int main(void)
{
    // Process A
    cout << "Enter number of producers: ";
    cin >> no_of_producers;
    cin >> no_of_producers;

    // Init Shared Memory

    // Init Random tree (300-500)    
    //      Maximum completion time 250 ms

    // Spawn Producer Threads
    //      Runs for random time between 10 to 20 secs
    //      Add a child to a non-running node
    //      Sleep for random time between 0 to 500 ms.



    // Spawn Process B

        // Spawn Consumer Threads

}