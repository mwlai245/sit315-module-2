// Import necessary modules
#include <stdio.h> 
#include <stdlib.h> 
#include <chrono> 
#include <pthread.h> 

using namespace std;
using namespace chrono;

// Define a maximum size for the matrices 
#define SIZE 10

// Define the maximum number of threads to use
#define MAX_THREADS 4

// Create three matrices with a size of SIZE*SIZE
int matrixA[SIZE][SIZE];
int matrixB[SIZE][SIZE];
int matrixC[SIZE][SIZE];

int steps = 0;

// Function to multiply sections of matrixC depending on thread
void* multiply(void* arg)
{
  int core = steps++;

  for (int i = core * SIZE / MAX_THREADS; i < (core + 1) * SIZE / MAX_THREADS; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      for (int k = 0; k < SIZE; k++)
      {
        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
      }
    }
  }
}

// Function to fill a given matrix with random values between 0 and 10
void createMatrix(int matrix[SIZE][SIZE])
{
  for (int i = 0; i < SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      matrix[i][j] = rand() % 10;
    }
  }
}

//Prints matrix row by row
void printMatrix(int matrix[SIZE][SIZE])
{
  for (int i = 0; i < SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      printf("%i", matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

int main()
{
  srand((unsigned int)time(NULL));

  // Defining threads
  pthread_t threads[MAX_THREADS];
  int i;

  // Creating matrix A and B
  createMatrix(matrixA);
  createMatrix(matrixB);

  // Print matrix A
  printf("Matrix A: \n");
  printMatrix(matrixA);

  // Print matrix B
  printf("Matrix B: \n");
  printMatrix(matrixB);

  auto start = high_resolution_clock::now();

  // Creating the threads
  for (int i = 0; i < MAX_THREADS; i++)
  {
    int* p;
    pthread_create(&threads[i], NULL, multiply, (void*)(p));
  }

  // Joining and waiting for threads to finish
  for (int i = 0; i < MAX_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // Record end time
  auto finish = chrono::high_resolution_clock::now();

  // Printing matrix C
  printf("Matrix A * B: \n");
  printMatrix(matrixC);

  // Calculate and print the time take to multiply matrices A and B
  auto duration = chrono::duration_cast<chrono::microseconds>(finish - start);
  printf("Time taken to multiply matrix A & B: %li microseconds.\n", duration.count());

  return 0;
}