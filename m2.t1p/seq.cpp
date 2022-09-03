#include <stdio.h>
#include <stdlib.h> 
#include <chrono> 
#include <fstream>
#include <omp.h>

using namespace std;
using namespace chrono;

// Define a maximum size for the matrices 
#define SIZE 10

// Create three matrices with a size of MAX * MAX
int matrixA[SIZE][SIZE];
int matrixB[SIZE][SIZE];
int matrixC[SIZE][SIZE];

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
  // rand() will always produce the same result unless a seed is set
  srand((unsigned int)time(NULL));

  // Creating random matrices A and B in a MAX-square matrix
  for (int i = 0; i < SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      // Assign row i, column j a random number between 0 and 10 in matrices A and B
      matrixA[i][j] = rand() % 10;
      matrixB[i][j] = rand() % 10;
    }
  }

  // Print matrix A
  printf("Matrix A: \n");
  printMatrix(matrixA);

  // Print matrix B
  printf("Matrix B: \n");
  printMatrix(matrixB);

  // Start time
  auto start = high_resolution_clock::now();

  // Calculate Matrix A * B as Matrix C
  for (int i = 0; i < SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      for (int k = 0; k < SIZE; k++)
      {
        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
      }
    }
  }

  auto finish = high_resolution_clock::now();

  // Print matrix C
  printf("Matrix A * B: \n");
  printMatrix(matrixC);

  // Calculate and print the time take to multiply matrices A and B
  auto duration = duration_cast<microseconds>(finish - start);
  printf("Time taken to multiply matrix A & B: %li microseconds.\n", duration.count());

  return 0;
}
