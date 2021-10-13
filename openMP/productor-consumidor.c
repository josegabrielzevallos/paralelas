#include "omp.h"
#include <malloc.h>
#include <stdio.h>

#define N        10000

/* Constantes para generar numeros aleatorios (numerical recipies) */
#define SEED       2531
#define RAND_MULT  1366
#define RAND_ADD   150889
#define RAND_MOD   714025
int randy = SEED;
int flag = 0;
int number =10 ;
int consumidores=1;
int productores=10;
int puntero_productor=0;



/* llenar un array con numeros aleatorios (Productor) */
void fill_rand(int index, double a[])
{
     randy = (RAND_MULT * randy + RAND_ADD) % RAND_MOD;
     a[index] = ((double) randy)/((double) RAND_MOD);
      
}

/* Sumar los elementos de un array (Consumidor) */
double Sum_array_element(int indice,double array[], double suma)
{
  #pragma omp critical
  suma+=array[indice];
}
  
int main()
{
  double *A, suma, tiempo;
  int i;
  int puntero_privado;
  omp_set_nested(1);
  A = (double *)malloc(number*sizeof(double));

  tiempo = omp_get_wtime();

#pragma parallel
  {
    #pragma omp parallel for  private(puntero_privado)
    for(i=0;i<productores;i++){
  
      if(i%2==0){
      	while(1){
	  while(puntero_privado<0||puntero_privado>9){
	    while(puntero_productor>9){
             #pragma omp flush(puntero_productor)
	    }
	    #pragma omp critical
	    {
	      puntero_privado = puntero_productor;
	      puntero_productor++;
	      printf("Puntero Productor =%i ha llegado a producir\n",puntero_privado);
              #pragma omp flush(puntero_productor)
	    }
	  }
	  fill_rand(puntero_productor,A);
          #pragma omp flush(A)
	}
      }
      else{
	while(1){
	  while(puntero_productor<0){
           #pragma omp flush(puntero_productor)
	  }
          #pragma omp critical
	  {
	  puntero_privado = puntero_productor;
	  puntero_productor--;
          #pragma omp flush(puntero_productor)
	  }
	  printf("Puntero Productor =%i ha llegado a consumir\n",puntero_privado);
	  Sum_array_element(puntero_privado,A,suma);
          #pragma omp flush(suma)
	  printf("El valor de suma actual es %d\n",suma);  
	}
      }
    }
 
  }
  tiempo = omp_get_wtime() - tiempo;

  printf(" La suma es %f en %f segundos\n",suma, tiempo);
}