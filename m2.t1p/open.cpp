#include <stdio.h> 
#include <stdlib.h> 
#include <chrono> 
#include <omp.h> 

// Set the namespace to reduce code (std::module...)
using namespace std;
using namespace chrono;

// Define a maximum number of threads
#define NUM_THREADS 4

// Define a maximum size for the matrices 
#define SIZE 10

// Create three matrices with a size of SIZE*SIZE
int matrixA[SIZE][SIZE];
int matrixB[SIZE][SIZE];
int matrixC[SIZE][SIZE];

// Function to multiply a section of matrixC based on thread number
void multiply(int matrixA[SIZE][SIZE], int matrixB[SIZE][SIZE], int matrixC[SIZE][SIZE])
{
  #pragma omp parallel for 
  for (int i = 0; i <SIZE; i++)
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

// Function to create a matrix with random values between 0 and 10
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
      printf("%i ", matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

int main()
{
  // Set the number of threads equal to the defined NUM_THREADS
  omp_set_num_threads(NUM_THREADS);

  // rand() will always produce the same result unless a seed is set
  srand((unsigned int)time(NULL));

  // Create matrix A and B
  createMatrix(matrixA);
  createMatrix(matrixB);

  // Print matrix A
  printf("Matrix A: \n");
  printMatrix(matrixA);

  // Print matrix B
  printf("Matrix B: \n");
  printMatrix(matrixB);

  auto start = high_resolution_clock::now();

  multiply(matrixA, matrixB, matrixC);

  auto finish = high_resolution_clock::now();

  // Print matrix C
  printf("Matrix A * B: \n");
  printMatrix(matrixC);

  // Calculate and print the time taken to multiply matrices A and B
  auto duration = duration_cast<microseconds>(finish - start);
  printf("Time taken to multiply matrix A & B: %li microseconds.\n", duration.count());

  return 0;
}