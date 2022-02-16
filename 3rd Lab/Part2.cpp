#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

class ComputationJob
{
    // int producer_no;
    // int status;
    // int matrix_id;
    // int matrix[1000][1000];

    int* producer_no;
    int* status;
    int* matrix_id;
    int** matrix;

public:

    static const int memory_size = 3 * sizeof(int) + 1000 * 1000 * sizeof(int);

    void* interpret(void* cj)
    {
        int* producer_no = (int*) cj;
        int* status = (int*) producer_no + sizeof(int);
        int* matrix_id = (int*) status + sizeof(int);
        int** matrix = (int**) matrix_id + sizeof(int);
        return (void*) matrix + 1000 * 1000 * sizeof(int);
    }

};

class CommonSpace 
{
    // int jobs_created;
    // int front;
    // int rear;
    // ComputationJob queue[8];
    // ComputationJob inProgress;
    
    int *jobs_created;
    int *front;
    int *rear;
    ComputationJob queue[8];
    // ComputationJob inProgress;



public:

    static const int memory_size = 3 * sizeof(int) + 8 * ComputationJob::memory_size;

    void interpret(void* cs)
    {
        jobs_created = (int*) cs;
        front = (int*) jobs_created + sizeof(int);
        rear = (int*) front + sizeof(int);

        void* curr = rear;
        for (int i = 0; i < 8; i++)
        {
            void* next = queue[i].interpret(curr);
            curr = next;
        }
        // inProgress.interpret(curr);
    }

};

class Worker
{
    int id;
    key_t key;

    void run()
    {
        sleep(5);
    }

public:

    Worker(int id, key_t key) : id(id), key(key)
    {
        cout << "Worker " << id << " : Started" << endl;;
        run();
    }

    ~Worker()
    {
        cout << "Worker " << id << " : Stopped" << endl;;
        exit(0);
    }
};

class Producer
{
    int id;
    key_t key;

    void run() 
    {
        sleep(2);
    }

public:

    Producer(int id, key_t key) : id(id), key(key)
    {
        cout << "Producer " << id << " : Started" << endl;;
        run();
    }

    ~Producer()
    {
        cout << "Producer " << id << " : Stopped" << endl;;
        exit(0);
    }

};

class Parent
{
    int NP;
    int NW;
    int NM;

    key_t key = 12121;
    int shmid;
    CommonSpace* cs;

public:
    void input()
    {
        cout << "Enter number of produces: ";
        cin >> NP;
        cout << endl;

        cout << "Enter number of workers: ";
        cin >> NW;
        cout << endl;

        cout << "Enter number of matrices: ";
        cin >> NM;
        cout << endl;

    }

    void createSHM()
    {
        shmid = shmget(key, sizeof(CommonSpace), 0666|IPC_CREAT);
        cs = (CommonSpace*) shmat(shmid, NULL, 0);
    }

    void closeSHM()
    {

    }

    void start()
    {
        cout << "Starting " << NP << " Producers...\n";

        for (int i = 0; i < NP; i++)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                Producer p(i+1, key);
            }
        }

        cout << "Starting " << NW << " Workers...\n";

        for (int i = 0; i < NW; i++)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                Worker w(i+1, key);
            }
        }
    }

    bool done()
    {
        return false;
    }

};

int main(void)
{
    Parent p;
    p.input();
    p.createSHM();
    p.start();
    while (p.done())
    {

    }
    sleep(7);
    p.closeSHM();
}