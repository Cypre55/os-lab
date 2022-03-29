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

// TODO Max number of children that can be added to a node
#define MAX_CHILD_JOBS 1000

#define DONE 2

#define MAX_TREE_SIZE 500
#define MIN_TREE_SIZE 300

// TODO: Log Steps like Produced, started, completed

using namespace std;

struct Node
{
    int job_id;
    int t2c; // In milliseconds

    int parent_ind;
    int child_jobs[MAX_CHILD_JOBS];
    int num_child_active;

    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;

    // Status
    // 0: Not Started
    // 1: On Going
    // 2: Done
    int status;

};

struct SharedMemoryBlock
{
    Node *array;
    int max_size;
    int total_nodes;
    int finished_producing;
    int completed_nodes;

    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
};

SharedMemoryBlock *block;

void *producer_function(void *arg)
{
    // hardcoded here
    int running_time = rand() % 11 + 10;
    cout << "Producer Waiting Time: " << running_time << endl;

    time_t start_time, current_time;
    time(&start_time);

    while (difftime(time(&current_time), start_time) < running_time)
    {
        //      Runs for random time between 10 to 20 secs
        //      Add a child to a non-running node
        //      Sleep for random time between 0 to 500 ms.
        while (true)
        {
            if(block->finished_producing == 2)
            {
                pthread_mutex_unlock(&block->mutex);
                return nullptr;
            }

            pthread_mutex_lock(&block->mutex);
            int N = block->total_nodes;
            block->total_nodes++;
            
            int node_index = rand() % N;

            // pthread_mutex_lock(&block->array[node_index].mutex);

            if (block->array[node_index].status == 0)
            {
                // TODO properly select job_id
                
                // pthread_mutex_lock(&block->array[N].mutex);
                int new_job_id = rand() % 100000000 + 1;
                block->array[N].job_id = new_job_id; 
                block->array[N].parent_ind = node_index;
                block->array[N].status = 0;
                block->array[N].t2c = rand() % 251;
                block->array[N].num_child_active = 0;
                cout << "Created: Job created with id " << new_job_id << endl;

                // pthread_mutex_unlock(&block->array[N].mutex);

                block->array[node_index].child_jobs[block->array[node_index].num_child_active] = N;
                block->array[node_index].num_child_active++;

                block->total_nodes++;

                break;
            }
            // pthread_mutex_unlock(&block->array[node_index].mutex);
            pthread_mutex_unlock(&block->mutex);
        }
        
        int sleep_time = rand() % 501;
        usleep(sleep_time * 1000);
    }

    pthread_mutex_unlock(&block->mutex);
    return nullptr;
}

int dfs(int node_index)
{
    int x = node_index;
    for (int i = 0; i < block->array[x].num_child_active; i++)
    {
        int child = block->array[x].child_jobs[i];
        if (block->array[child].status == 0)
        {
            x = dfs(child);
        }
    }
    return x;
}

int dfs2(int x)
{
    int count = 1;
    for (int i = 0; i < block->array[x].num_child_active; i++)
    {
        int child = block->array[x].child_jobs[i];
        count += dfs2(child);
    }
    return count;
}

void *consumer_function(void* arg)
{
    while(true)
    {
        // cout << "Yo\n";
        // pthread_mutex_lock(&block->mutex);
        // cout << "Yello\n";

        if (block->completed_nodes == block->total_nodes && block->finished_producing == 1)
        {
            // pthread_mutex_unlock(&block->mutex);
            exit(0);
            break;
        }
        block->completed_nodes++;
        // pthread_mutex_unlock(&block->mutex);

        int node_index;
        // DFS

        // start from the root
        node_index = 0;
        node_index = dfs(node_index);


        pthread_mutex_lock(&block->array[node_index].mutex);
        block->array[node_index].status = 1;
        pthread_mutex_unlock(&block->array[node_index].mutex);
        cout << "In Progress: Job " << block->array[node_index].job_id << " began executing.\n";
        cout.flush();

        usleep(block->array[node_index].t2c * 1000);

        pthread_mutex_lock(&block->array[node_index].mutex);
        block->array[node_index].status = 2;
        pthread_mutex_unlock(&block->array[node_index].mutex);
        cout << "Completion: Job " << block->array[node_index].job_id << " completed execution.\n";
        cout.flush();

        if (node_index == 0)
        {
            cout << "All Jobs Ran\n";
            block->finished_producing = 2;
            exit(0);
        }

    }

    return nullptr;
}

int main(void)
{
    srand(time(NULL));
    // Process A
    cout << "Enter number of producers: ";
    cin >> no_of_producers;
    cout << "Enter number of workers: ";
    cin >> no_of_consumers;

    // Init Shared Memory

    int init_size = rand() % (MAX_TREE_SIZE - MIN_TREE_SIZE + 1) + MIN_TREE_SIZE;
    int max_nodes = init_size + 1000 * no_of_producers;

    key_t key_ = ftok(".", 0);
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedMemoryBlock), IPC_CREAT | 0666);
    block = (SharedMemoryBlock *) shmat(shmid, NULL, 0);

    key_t key = ftok("./t", 1);
    int node_id = shmget(IPC_PRIVATE, sizeof(Node) * max_nodes, IPC_CREAT | 0666);
    block->array = (Node *) shmat(node_id, NULL, 0);
    block->max_size = max_nodes;
    block->finished_producing = 0;


    // Init Mutexes
    

    pthread_mutexattr_init(&block->attr);
    pthread_mutexattr_settype(&block->attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    pthread_mutexattr_setpshared(&block->attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&block->mutex, &block->attr);

    for (int i = 0; i < max_nodes; i++)
    {
        pthread_mutexattr_init(&block->array[i].attr);
        pthread_mutexattr_settype(&block->array[i].attr, PTHREAD_MUTEX_ERRORCHECK_NP);
        pthread_mutexattr_setpshared(&block->array[i].attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&block->array[i].mutex, &block->array[i].attr);
    }
    // cout << "Hi\n";
    // cout << block->array << endl;
    // cout << "Hello\n";


    // Init Random tree (300-500)    
    //      Maximum completion time 250 ms

    block->array[0].job_id = 101010101; 
    block->array[0].parent_ind = -1;
    block->array[0].status = 0;
    block->array[0].t2c = rand() % 251;
    block->array[0].num_child_active = 0;

    block->total_nodes++;

    for (int i = 0; i < init_size - 1; i++)
    {
        int N;

        while (true)
        {
            N = block->total_nodes;
            int node_index = rand() % N;
            if (block->array[node_index].status == 0)
            {
                int new_job_id = rand() % 100000000 + 1;
                block->array[N].job_id = new_job_id; 
                block->array[N].parent_ind = node_index;
                block->array[N].status = 0;
                block->array[N].t2c = rand() % 251;
                block->array[N].num_child_active = 0;

                block->array[node_index].child_jobs[block->array[node_index].num_child_active] = N;
                block->array[node_index].num_child_active++;

                block->total_nodes++;

                break;
            }
        }
    }

    // cout << "init size is " << init_size << endl;
    // cout << "tree size is " << dfs2(0) << endl;

    // Spawn Producer Threads
    

    pthread_t producers[no_of_producers];

    for (int i = 0; i < no_of_producers; i++)
    {
        pthread_create(&producers[i], nullptr, &producer_function, nullptr);
    }



    int pid = fork();
    if (pid == 0)
    {
        cout << "\n\t\t New Process Created.\n";
        pthread_t consumers[no_of_consumers];
        block = (SharedMemoryBlock *) shmat(shmid, nullptr, 0);
        for (int i = 0; i < no_of_consumers; i++)
        {
            pthread_create(&consumers[i], nullptr, &consumer_function, nullptr);
        }

        for (int i = 0; i < no_of_consumers; i++)
        {
            pthread_join(consumers[i], nullptr);
        }

        cout << "All Jobs finished running.\n";
        shmdt(block);
        exit(0);
    }

    for (int i = 0; i < no_of_producers; i++)
    {
        pthread_join(producers[i], nullptr);
    }

    pthread_mutex_lock(&block->mutex);
    block->finished_producing = 1;
    cout << "All producers finished publishing.\n";
    pthread_mutex_unlock(&block->mutex);


    shmdt(block);

    shmctl(shmid, IPC_RMID, nullptr);

    return 0;
}

