#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
void Read_vector(double local_a[], int local_n, int n, char vec_name[], int my_rank, MPI_Comm comm){
  double* a = NULL;
  int i;

  if(my_rank == 0){
    a = malloc(n*sizeof(double));
    printf("vector: %s\n", vec_name);
    for(i = 0; i < n; i++){
      a[i] = rand() % 100000;
    }
    MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE,0,comm);
    free(a);
  }
  else{
      MPI_Scatter(a,local_n,MPI_DOUBLE,local_a,local_n,MPI_DOUBLE,0,comm);
  }
}

void Print_vector(double local_b[], int local_n, int n, char title[], int my_rank, MPI_Comm comm){
  double* b = NULL;
  //printf("---------print-------\n");
  int i;
  if(my_rank == 0){
    b = malloc(n*sizeof(double));
    MPI_Gather(local_b, local_n, MPI_DOUBLE,b,local_n, MPI_DOUBLE,0,comm);
    printf("%s\n", title);
    for(i = 0; i < n; i++){
      printf("%f ", b[i]);
    }
    printf("\n");
    free(b);
  }
  else{
    MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0, comm);
  }
}

void Mat_vect_mult(double local_A[], double local_x[], double local_y[], int local_m, int n, int local_n, MPI_Comm comm){
  double* x;
  int local_i,j;
  int local_ok=1;

  x = malloc(n*sizeof(double));
  MPI_Allgather(local_x, local_n, MPI_DOUBLE, x, local_n, MPI_DOUBLE, comm);
  for(local_i = 0; local_i < local_m; local_i++){
    local_y[local_i] = 0.0;
    for(j = 0; j <n; j++){
      local_y[local_i] += local_A[local_i*n+j] * x[j];

    }
  }
  //printf("vector_y->> %lf\n",local_y[0]);
  free(x);
}

int main(){
  srand(time(0));
  printf("--------THIS--------");
  int comm_sz;
  int my_rank;
  double start,finish;
  int n=100,m=100, local_n,local_m;
  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  local_n = n/comm_sz; //columns
  local_m = m/comm_sz; //rows



  double local_x[local_n],local_y[local_m],local_A[local_n*local_m];
  Read_vector(local_A,local_n*local_m,n*m,"A",my_rank,MPI_COMM_WORLD);
  Read_vector(local_x,local_n,n,"x",my_rank,MPI_COMM_WORLD);
  
  start = MPI_Wtime();
  Mat_vect_mult(local_A, local_x, local_y, local_m,n,local_n,MPI_COMM_WORLD);
  finish = MPI_Wtime();
  
//   Print_vector(local_y,local_m,m,"y", my_rank,MPI_COMM_WORLD);
   printf("Proc %d > Elapsed time = %e seconds\n", my_rank, finish-start);
  MPI_Finalize();
  return 0;
}
