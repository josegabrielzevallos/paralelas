#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

#define ROWS 500
#define COLUMNS 500
#define N_THREADS 10

int mat1[ROWS][COLUMNS],mat2[ROWS][COLUMNS],res_mat[ROWS][COLUMNS];

void *mult_thread(void *t)
{   
    /*This function calculates 50 ROWS of the matrix*/

    int starting_row;
    starting_row = *((int *)t);
    starting_row = 50 * starting_row;

    int i,j,k;
    for (i = starting_row;i<starting_row+50;i++)
        for (j=0;j<COLUMNS;j++)
            for (k=0;k<ROWS;k++)
                res_mat[i][j] += (mat1[i][k] * mat2[k][j]);
    return;
}

void fill_matrix(int mat[ROWS][COLUMNS])
{
    int i,j;
    for(i=0;i<ROWS;i++)
        for(j=0;j<COLUMNS;j++)
            mat[i][j] = 1;
}

int main()
{
    int n_threads = 10; //10 threads created bcos we have 500 rows and one thread calculates 50 rows
    int j=0;
    pthread_t p[n_threads];

    fill_matrix(mat1);
    fill_matrix(mat2);
    for (j=0;j<10;j++)
        pthread_create(&p[j],NULL,mult_thread,&j);

    for (j=0;j<10;j++)
        pthread_join(p[j],NULL);

    printf("%d\n",res_mat[0][0]);
    return 0;
}