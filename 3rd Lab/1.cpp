#include <iostream>
#include <sys/shm.h>
#include <unistd.h>

using namespace std;

typedef struct _process_data
{
    double **A;
    double **B;
    double **C;
    int veclen, i, j;
}
ProcessData;

void *mult(void *arg)
{
    ProcessData pd = *((ProcessData *) arg);
    // cout << "pid of process calculating C[" << pd.i << "][" << pd.j << "] is " << getpid() << endl;
    double result = 0;
    for (int i = 0; i < pd.veclen; i++)
    {
        result += pd.A[pd.i][i] * pd.B[i][pd.j];
        // cout << getpid() << " " << result << endl;
    }
    pd.C[pd.i][pd.j] = result;
    return NULL;
}

int main()
{
    cout << "Enter the dimensions for the matrix A" << endl;
    int ar, ac;
    cin >> ar >> ac;
    cout << "Enter the dimensions for the matrix B" << endl;
    int br, bc;
    cin >> br >> bc;
    if (ac != br)
    {
        cout << "The matrices are not compatible for multiplication" << endl;
        exit(0);
    }
    // we need (ar * ac + br * bc + ar * bc) * sizeof(double) amount of shared memory
    size_t size = (ar * ac + br * bc + ar * bc) * sizeof(double) + (ar + br + ar) * sizeof(double *);
    // cout << "size is " << size << endl;

    int shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shm");
    }

    void *shared_memory = shmat(shmid, NULL, 0);
    // storing the 2d arrays in contiguous blocks of memory
    double **A = (double **) shared_memory;
    for (int i = 0; i < ar; i++)
    {
        A[i] = (double *) ((char *) A + ar * sizeof(double *) + i * ac * sizeof(double));
    }
    // for (int i = 0; i < ar; i++)
    // {
    //     printf("%u\t", A[i]);
    // }
    // printf("\n");
    double **B = (double **) ((char *) A + ar * sizeof(double *) + ar * ac * sizeof(double));
    for (int i = 0; i < br; i++)
    {
        B[i] = (double *) ((char *) B + br * sizeof(double *) + i * bc * sizeof(double));
    }
    // for (int i = 0; i < br; i++)
    // {
    //     printf("%u\t", B[i]);
    // }
    // printf("\n");
    double **C = (double **) ((char *) B + br * sizeof(double *) + br * bc * sizeof(double));
    for (int i = 0; i < ar; i++)
    {
        C[i] = (double *) ((char *) C + ar * sizeof(double *) + i * bc * sizeof(double));
    }
    // for (int i = 0; i < ar; i++)
    // {
    //     printf("%u\t", C[i]);
    // }
    // printf("\n");
    
    // printf("A, B, C are %u, %u, %u\n", A, B, C);

    cout << "A:" << endl;
    for (int i = 0; i < ar; i++)
    {
        for (int j = 0; j < ac; j++)
        {
            cin >> A[i][j];
        }
    }
    
    cout << "B:" << endl;
    for (int i = 0; i < br; i++)
    {
        for (int j = 0; j < bc; j++)
        {
            cin >> B[i][j];
        }
    }

    ProcessData pd;
    pd.A = A;
    pd.B = B;
    pd.C = C;
    pd.veclen = ac;
    
    for (int i = 0; i < ar; i++)
    {
        for (int j = 0; j < bc; j++)
        {
            pd.i = i;
            pd.j = j;
            if (fork() == 0)
            {
                cout << "pid: " << getpid() << " started" << endl;
                void *addr = shmat(shmid, NULL, 0);
                mult(&pd);
                shmdt(addr);
                cout << "pid: " << getpid() << " ended" << endl;
                exit(0);
            }
        }
    }

    // wait for all the child processes
    while (wait(NULL) > 0);

    cout << "C:" << endl;
    for (int i = 0; i < ar; i++)
    {
        for (int j = 0; j < bc; j++)
        {
            cout << C[i][j] << " ";
        }
        cout << endl;
    }

    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
