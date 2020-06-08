/*
Student Name: Omer Faruk Dogru
Student Number: 2016400288
Compile Status: Compiling
Program Status: Working
Notes: you should give arraysize as last argument such as:

mpirun -np 11  --hostfile hostfile mpi_project 10000
*/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MASTER 0 /* taskid of first process */
void shuffle(int *array, int n);
int is_arms(int n);
long power(int r, int i);
void sort(int *arr, int ct);

int main(argc, argv) int argc;
char *argv[];
{
    clock_t begin = clock();

    int a = atoi(argv[1]); //arraysize given as argument
    int arr[a];
    int res[a];
    int numbers[1000];
    MPI_Status status;

    int rc, numtasks, taskid;
    rc = MPI_Init(&argc, &argv);
    rc |= MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    rc |= MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    int numworkers = numtasks - 1;
    int chunksize = (a / numworkers);
    int ct = 0;
    int allNumbers[1000]; //keeps all armstrong numbers found in processes

    if (taskid == MASTER)
    {
        int total;
        int index = 0;
        int index2 = 0;
        int index3 = 0;
        //printf("%d.. \n", a);
        for (int i = 0; i < a; i++) // initialize array
        {
            arr[i] = i;
        }
        for (size_t i = 0; i < 50; i++)
        {
            shuffle(&arr[0], a); // shuffle randomly array 50 times
        }

        for (int dest = 1; dest <= numworkers; dest++) // send partitions to its correspending worker
        {
            MPI_Send(&index, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);

            MPI_Send(&arr[index], chunksize, MPI_FLOAT, dest, 2, MPI_COMM_WORLD);

            index += chunksize;
        }

        //recieve operations
        for (int source = 1; source <= numworkers; source++) // receives arrays from workers which includes armstrong numbers
        {
            MPI_Recv(&index2, 1, MPI_INT, source, 4, MPI_COMM_WORLD, &status);

            MPI_Recv(&numbers[0], index2, MPI_FLOAT, source, 5, MPI_COMM_WORLD, &status);
            //index3 += index2;
            //printf("index2 : %d \n", index2);
            for (int i = 0; i < index2; i++)
            {
                if (numbers[i] != 0)
                {
                    allNumbers[ct++] = numbers[i];
                    //printf("%d \n", numbers[i]);
                    //ct++;
                }
            }
        }
        int sorted[ct]; // initialize array which will contain all armstrong numbers in order
        for (int i = 0; i < ct; i++)
        {
            sorted[i] = allNumbers[i];
        }
        sort(&sorted[0], ct); // sorting

        FILE *f = fopen("armstrong.txt", "w"); // initialize file
        for (int i = 0; i < ct; i++)           // writes numbers to file
        {
            fprintf(f, "%d\n", sorted[i]);
        }

        MPI_Recv(&total, 1, MPI_INT, numworkers, 3, MPI_COMM_WORLD, &status); // lastly receives sum from last worker
        printf("MASTER: Sum of all Armstrong numbers = %d \n", total);

        clock_t end = clock(); // to calculate execution time of program
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("execution time is %f \n", time_spent);
    }

    if (taskid > MASTER) // workers
    {
        int index;
        int check = 0;
        int nums[100];
        int index2 = 0;
        int sum = 0;
        int receivedSum = 0;

        //each worker receives its tasks from master
        MPI_Recv(&index, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD, &status);

        MPI_Recv(&res[index], chunksize, MPI_FLOAT, MASTER, 2, MPI_COMM_WORLD, &status);

        for (int i = 0; i < chunksize; i++) // iterate over array to find armstrong number and their sum
        {
            check = is_arms(res[index]); // check for armstrong
            if (check)
            {
                sum += check; // sum
                //printf("task id %d , found %d \n", taskid, check);

                nums[index2] = check; // adds armstrong numbers to arrray
                index2++;
            }
            index++;
        }
        //send armstrong numbers to master
        MPI_Send(&index2, 1, MPI_INT, MASTER, 4, MPI_COMM_WORLD);
        MPI_Send(&nums[0], index2, MPI_FLOAT, MASTER, 5, MPI_COMM_WORLD);

        printf("Sum of Armstorng numbers in Process %d = %d \n", taskid, sum);

        if (taskid != 1) // receives sums from previous worker except 1 since first worker does not have previous worker
        {
            MPI_Recv(&receivedSum, 1, MPI_INT, taskid - 1, 3, MPI_COMM_WORLD, &status);
        }

        sum += receivedSum;       // adds received sum to founded sum
        if (taskid != numworkers) // send to next worker
        {
            MPI_Send(&sum, 1, MPI_INT, taskid + 1, 3, MPI_COMM_WORLD);
        }

        if (taskid == numworkers) // last worker sends sum to master
        {
            MPI_Send(&sum, 1, MPI_INT, MASTER, 3, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
}

void shuffle(int *array, int n) // shuffles an array randomly
{
    srand(time(NULL));

    for (int i = 0; i < n - 1; i++)
    {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

int is_arms(int n) //checks if a number is armstrong. if it is returns number, otherwise returns 0
{
    long sum = 0;
    int count = 0;
    int temp = n;

    while (temp != 0)
    {
        count++;
        temp /= 10;
    }

    temp = (long)n;
    while (temp != 0)
    {
        int r = temp % 10;
        sum += power(r, count);
        temp /= 10;
    }
    if (sum == n)
    {
        return n;
    }
    return 0;
}

long power(int r, int i) // power r^i
{
    long p = 1;
    for (int j = 0; j < i; j++)
    {
        p *= r;
    }
    return p;
}

void sort(int *arr, int ct) // sorts an array. complexity n^2
{
    for (int i = 0; i < ct; i++)
    {
        for (int j = 0; j < ct; j++)
        {
            if (arr[j] > arr[i])
            {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}