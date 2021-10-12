#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* the number of data elements in each process */
#define N 150000

int isSorted;

void secuencial(int arr[], int n)
{
    isSorted = 0;
    while (!isSorted) {
        isSorted = 1;
  
        for (int i = 1; i <= n - 2; i = i + 2) {
            if (arr[i] > arr[i + 1]) {
              int temp;
              temp = arr[i];
              arr[i] = arr[i + 1];
              arr[i+1] = temp;
                isSorted = 0;
            }
        }

        for (int i = 0; i <= n - 2; i = i + 2) {
            if (arr[i] > arr[i + 1]) {
               int temp;
              temp = arr[i];
              arr[i] = arr[i + 1];
              arr[i+1] = temp;
                isSorted = 0;
            }
        }
    }
  
    return;
}


void init(int* data, int rank) {
  int i;
  srand(rank);
  for (i = 0; i < N; i++) {
    data[i] = rand( ) % 100;
  }
}


void print(int* data, int rank) {
  int i;
  printf("Process %d: ");
  for (i = 0; i < N; i++) {
    printf("%d ", data[i]);
  }
  printf("\n");
}


int cmp(const void* ap, const void* bp) {
  int a = * ((const int*) ap);
  int b = * ((const int*) bp);

  if (a < b) {
    return -1;
  } else if (a > b) {
    return 1;
  } else {
    return 0;
  }
}

int max_index(int* data) {
  int i, max = data[0], maxi = 0;

  for (i = 1; i < N; i++) {
    if (data[i] > max) {
      max = data[i];
      maxi = i;
    }
  }
  return maxi;
}


int min_index(int* data) {
  int i, min = data[0], mini = 0;

  for (i = 1; i < N; i++) {
    if (data[i] < min) {
      min = data[i];
      mini = i;
    }
  }
  return mini;
}


void parallel_sort(int* data, int rank, int size) {
  int i;
  int other[N];
  for (i = 0; i < size; i++) {
    qsort(data, N, sizeof(int), &cmp);
    int partener;

    if (i % 2 == 0) {
      if (rank % 2 == 0) {
        partener = rank + 1;
      } else {
        partener = rank - 1;
      }
    } else {
      if (rank % 2 == 0) {
        partener = rank - 1;
      } else {
        partener = rank + 1;
      }
    }
    if (partener < 0 || partener >= size) {
      continue;
    }
    if (rank % 2 == 0) {
      MPI_Send(data, N, MPI_INT, partener, 0, MPI_COMM_WORLD);
      MPI_Recv(other, N, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
      MPI_Recv(other, N, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(data, N, MPI_INT, partener, 0, MPI_COMM_WORLD);
    }
    if (rank < partener) {
      while (1) {
        int mini = min_index(other);
        int maxi = max_index(data);
        if (other[mini] < data[maxi]) {
          int temp = other[mini];
          other[mini] = data[maxi];
          data[maxi] = temp;
        } else {
          break;
        }
      }
    } else {
      while (1) {
        int maxi = max_index(other);
        int mini = min_index(data);
        if (other[maxi] > data[mini]) {
          int temp = other[maxi];
          other[maxi] = data[mini];
          data[mini] = temp;
        } else {
         
          break;
        }
      }
    }
  }
}
int main() {

  int rank, size;

  double start,finish;
  double start1,finish1;

  int data[N];


  MPI_Init(NULL,NULL);


  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


  init(data, rank);

  start = MPI_Wtime();
  parallel_sort(data, rank, size);
  finish = MPI_Wtime();

  printf("time paralelo = %e seconds\n", finish-start);



  start1 = MPI_Wtime();
  secuencial(data, size);
  finish1 = MPI_Wtime();
  printf("time secuencial = %e seconds\n", finish1-start1);
  
  MPI_Finalize( );
  return 0;
}
